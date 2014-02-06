/*
 *          ppMotionBlur.glsl
 *
 *    This technique is described in the 2003 GDC presentation 
 *      "Stupid OpenGL Shader Tricks" by Simon Green
 *
 *
 *    Each moblurred object must be rendered two times :
 *      # 1st pass: RENDER every object, DEPTH WRITING ON.
 *      # 2nd pass: RENDER only moblurred objects, DEPTH WRITING OFF => 
 *                        fill moblur vector buffer
 *                        depth test : GL_LEQUAL
 *
 *    This is the 2nd pass program shader.
 *
 *    note: may have problems with blended objects (not tested)
 */

//------------------------------------------------------------------------------


-- Vertex

/**
 *  2nd Pass vertex shader (works for a Forward / Deferred pipeline)
 */


// IN
layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;

// OUT
out vec2 vMotionVelocity;

// UNIFORM
uniform mat4 uModelViewProj;
uniform mat4 uModelViewProj_prev;
uniform mat4 uModelView;
uniform mat4 uModelView_prev;
uniform mat3 uNormalViewMatrix;
//uniform float uBlurScale = 1.0f;


void main()
{
  // Compute previous and current position in Eye-Space
  vec4 mvPos      = uModelView      * inPosition;
  vec4 mvPos_prev = uModelView_prev * inPosition;

  // Compute normal in Eye-Space
  vec3 mvNormal = uNormalViewMatrix * inNormal;

  // Compute motion vector in Eye-space
  vec3 mvMotion = (mvPos - mvPos_prev).xyz;

  // Compute previous and current position in clip-Space
  vec4 mvpPos      = uModelViewProj      * inPosition;
  vec4 mvpPos_prev = uModelViewProj_prev * inPosition;

  // Interpolate the previous position to control the effect's strength
  //mvpPos_prev = mix( mvpPos, mvpPos_prev, uBlurScale);

  // Matthias Wlocka's trick : use the current or previous transformation depends on
  // motion direction with respect to the vertex normal.
  float dp = dot( mvNormal, mvMotion);
  gl_Position = (dp > 0.0f)? mvpPos : mvpPos_prev;

  // Retrieve the Normalized Device Coordinates (NDC) [for OpenGL, in the (-1,-1,-1) (1,1,1) cube]
  mvpPos      /= mvpPos.w;
  mvpPos_prev /= mvpPos_prev.w;

  // Compute Screen-Space velocity
  vMotionVelocity = (mvpPos - mvpPos_prev).xy;
  //vMotionVelocity *= 0.5f * uViewport;  // moved in the final pass
}


--


//------------------------------------------------------------------------------


-- Instanced.Vertex

/**
 *  2nd Pass vertex shader (works for a Forward / Deferred pipeline)
 *  for instanced geometry
 */


// IN
layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 5) in mat4 inModelMatrix;
layout(location = 9) in mat3 inNormalMatrix;
layout(location = 12) in mat4 inModelMatrix_prev;

// OUT
out vec2 vMotionVelocity;

// UNIFORM
uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
//uniform float uBlurScale = 1.0f;


void main()
{
  // Compute previous and current position in Eye-Space
  vec4 mvPos      = uViewMatrix * inModelMatrix * inPosition;
  vec4 mvPos_prev = uViewMatrix * inModelMatrix_prev * inPosition;

  // Compute normal in Eye-Space
  vec3 mvNormal = mat3(uViewMatrix) * inNormalMatrix * inNormal;

  // Compute motion vector in Eye-space
  vec3 mvMotion = (mvPos - mvPos_prev).xyz;

  // Compute previous and current position in clip-Space
  vec4 mvpPos      = uProjectionMatrix * mvPos;
  vec4 mvpPos_prev = uProjectionMatrix * mvPos_prev;

  // Interpolate the previous position to control the effect's strength
  //mvpPos_prev = mix( mvpPos, mvpPos_prev, uBlurScale);

  // Matthias Wlocka's trick : use the current or previous transformation depends on
  // motion direction with respect to the vertex normal.
  float dp = dot( mvNormal, mvMotion);
  gl_Position = (dp > 0.0f)? mvpPos : mvpPos_prev;

  // Retrieve the Normalized Device Coordinates (NDC) [for OpenGL, in the (-1,-1,-1) (1,1,1) cube]
  mvpPos      /= mvpPos.w;
  mvpPos_prev /= mvpPos_prev.w;

  // Compute Screen-Space velocity
  vMotionVelocity = (mvpPos - mvpPos_prev).xy;
  //vMotionVelocity *= 0.5f * uViewport;  // moved in the final pass
}


--

//------------------------------------------------------------------------------


-- FillBuffer.Fragment

/**
 *  2nd pass fragment shader (only for a Deferred pipeline)
 */

// IN
in vec2 vMotionVelocity;

// OUT
layout(location = 2) out vec4 fragMotion;


void main()
{
  fragMotion.rg = vMotionVelocity;
}


--

//------------------------------------------------------------------------------


-- Fragment

/**
 *   2nd pass in a Forward pipeline
 *   OR
 *   Post-Process pass in a Deferred pipeline
 */

// IN
in vec2 vTexCoord;
in vec2 vMotionVelocity; // FORWARD

// OUT
layout(location = 0) out vec4 fragColor;

// UNIFORM
uniform sampler2D uSceneTex;
uniform sampler2D uMotionTex; // DEFERRED
uniform float uBlurScale = 1.0f;
uniform int uBlurNumSamples = 16;
uniform vec2 uViewport;

vec3 blurEffect( vec2 blurVector, const float nSamples)
{
/**
  Calculate a pixel color by sampling fragment along a vector.
  Note: a weight can be added wrt distance.
*/

  vec3 blurColor = vec3(0.0f);
  
  // Linear sampling step
  const float samplingStep = 1.0f / (nSamples-1.0f);
  
  for (int i=0; i<nSamples; ++i)
  {
    float k = i * samplingStep;
    vec2 uv = gl_FragCoord.xy - k * blurVector;

    blurColor += texture( uSceneTex, uv).rgb;
  }
  
  blurColor *= 1.0f / nSamples;
  
  return blurColor;
}

void main()
{
  #if 1 // DEFERRED
  vec2 motionVector = texture( uMotionTex, vTexCoord).rg;
  #else // FORWARD
  vec2 motionVector = vMotionVelocity;
  #endif

  // If not done in the vertex stage:
  motionVector *= 0.5f * uViewport;
  
  vec3 blurColor = blurEffect( uBlurScale*motionVector, uBlurNumSamples);
  
  fragColor = vec4( blurColor, 1.0f);
  fragColor = vec4( motionVector, 0.0f, 1.0f);//XXX
  
  // To see the effect:
  //vec2 nBlurColor = 0.5f * normalize(motionVector) + 0.5f;
  //fragColor.rgb *= vec3(nBlurColor, uBlurScale);
  //fragColor.rgb *= vec3(motionVector, uBlurScale);
  /**/
}

