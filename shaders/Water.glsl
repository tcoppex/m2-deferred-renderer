/*
 *          PassThrough.glsl
 *
 *      Simple real-time wave simulation on the vertex shader.
 *
 *      This algorithm can be used to generate tileable normalmap
 *      wave textures (with appropriate parameters)
 *
 *      A fresnel factor can be simply added (for reflection / refraction).
 *
 */

//------------------------------------------------------------------------------


-- Vertex


// IN
layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;//
layout(location = 2) in vec2 inTexCoord;

// OUT
out vec3 vNormalVS;
out vec4 vProjTexCoord;

// UNIFORM
uniform mat4 uModelViewProjMatrix;
uniform mat3 uNormalViewMatrix;
uniform mat4 uProjTexMatrix;
uniform vec3 uEye;
uniform float uTime;

struct waveData_t
{
  float amplitude;
  float frequency;
  float phase;
  vec2 direction;
};

#define MAX_WAVE  8
uniform waveData_t uWaves[MAX_WAVE];
uniform int uNumWave = MAX_WAVE;


void main()
{
  vec4 pos = vec4( inPosition.x, 0.0f, inPosition.z, 1.0f);
  
  vec3 normal = vec3( 0.0f, 1.0f, 0.0f);
  
  for (int i=0; i<uNumWave; ++i)
  {
    waveData_t wave = uWaves[i];
    
    float alpha = dot(wave.direction, pos.xz)*wave.frequency + uTime*wave.phase;
    
    // Simple wave simulation
    pos.y += wave.amplitude * sin( alpha );
    
    // Compute the normal
	  float delta = wave.frequency * wave.amplitude * cos( alpha );	
    normal.xz -= delta * wave.direction;
    
    /*
    //Gerstner crest effect
    float Q = 0.125f / wave.frequency;
  
    pos.xz += Q * wave.direction * cos( alpha );    
    // normal TODO
    */
  }
    
  // View-Space normal
  vNormalVS = uNormalViewMatrix * normal;
  vNormalVS = normalize( vNormalVS );
  
  vProjTexCoord = uProjTexMatrix * pos;
  //vProjTexCoord = uProjTexMatrix * pos + 2*vec4(vNormal,0);
  
  gl_Position = uModelViewProjMatrix * pos;
}


--

//------------------------------------------------------------------------------


-- Fragment


// IN
in vec3 vNormalVS;
in vec4 vProjTexCoord;

// OUT
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec3 fragNormal;

// UNIFORM
uniform sampler2D uReflectTex;
uniform vec4 uWaterColor = vec4( 0.0f, 0.3f, 0.5f, 1.0f);


void main()
{
  vec4 reflectColor = textureProj( uReflectTex, vProjTexCoord);
  fragColor = mix( reflectColor, vec4(0.0f,1.0f,1.0f,1.0f), 0.3f);
  fragColor.a = 1.0f;
  
  fragNormal = vNormalVS;
}

