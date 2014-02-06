/*
 *          PostProcess.glsl
 *
        This file contains the default PostProcess vertex shaders
        and the final / default post-process effect.
        
        It contains other simple post proc effects.
        
        List of effects :
            Vignetting (ok)
            MotionBlur (ok)
            Blur
            Bloom
            SSAO  (~ok)
            Depth Of Field            
            GammaCorrect (~ ok)
*/

//------------------------------------------------------------------------------


-- Vertex


out vec2 vTexCoord;

/*  
  This is an OpenGL port of a trick found at #AltDevBlogADay to create a triangle 
  where the texture is mapped to the window coords as a quad. 
  It needs 3 random vertex to be sent to the graphic card, the real vertice position 
  (and texCoord) are generated afterward.
  
  ref : http://altdevblogaday.com/2011/08/08/interesting-vertex-shader-trick/
*/
void main(void)
{
  vTexCoord.s = (gl_VertexID << 1) & 2;
  vTexCoord.t = gl_VertexID & 2;
  
  gl_Position = vec4( 2.0f*vTexCoord - 1.0f, 0.0f, 1.0f);
}


--

//------------------------------------------------------------------------------


-- Fragment


// IN
in vec2 vTexCoord;

// OUT
layout(location = 0) out vec4 fragColor;

// UNIFORM
uniform sampler2D uSceneTex;
uniform sampler2D uInputTex;//
uniform sampler2D uAOTex;
uniform sampler2D uBloomTex; // TODO: merge with AO (?)
uniform sampler2D uDoFTex;

uniform sampler2D uMotionTex; //XXX

uniform float uTime;
uniform float uRand;
uniform float uInvGamma = 0.4545f; // <=> 1.0f / 2.2f

uniform bool uEnableAO = false;
uniform bool uEnableBloom = false;
uniform bool uEnableDoF = false;
uniform bool uEnableVignetting = false;
uniform bool uEnableGammaCorrection = false;

uniform vec2 uTexelSize;



vec3 gammaCorrect(in vec3 inColor)
{
  // Linear to sRGB space
  return pow( inColor, vec3(uInvGamma));
}


float vignetting()
{ 
  float edgeDarkness = 2.5f + 0.1f*uRand;//abs(sin(0.5f*uTime))*(1.0f + 0.25f*uRand) + 1.0f;

  vec2 coords = (vTexCoord.xy - 0.5f);
  float vFactor = dot(coords, coords) * edgeDarkness;
    
  vFactor = max( 0.0f, 1.0f - vFactor);
  //vFactor = smoothstep( -0.15f, 0.30f, vFactor);
  
  return vFactor;
}


vec3 chromaticAberration()
{
  vec3 tex = vec3( 0.0f );
  vec2 uv;
  
  vec2 offset = uRand*200*sin(mod(uTime, 20.0f)) * sin(uRand-0.5f) * uTexelSize * vTexCoord.st;
  
  // YELLOW
  uv = vec2( -0.75f, 0.5f) * offset;
  tex.rg += texture( uSceneTex, vTexCoord + uv).rg;
  
  // CYAN
  uv = vec2( 0.4f, -1.1f) * offset;
  tex.gb += texture( uSceneTex, vTexCoord + uv).gb;
  
  // MAGENTA
  uv = vec2( -0.7f, 0.7f) * offset;
  tex.rb += texture( uSceneTex, vTexCoord + uv).rb;  
  
  return tex;
}


vec3 sepiaFilter(in vec3 inColor)
{
  vec3 outColor;
  
  outColor.r = dot( vec3(0.393f, 0.769f, 0.189f), inColor);
  outColor.g = dot( vec3(0.349f, 0.686f, 0.168f), inColor);
  outColor.b = dot( vec3(0.272f, 0.534f, 0.131f), inColor);

  //outColor = smoothstep( 0.0f, 1.0f, outColor);
  outColor = clamp( outColor, 0.0f, 1.0f);
  
  return outColor;
}


vec3 greyscaleFilter(in vec3 inColor)
{
  float greyscale = dot(vec3(0.299f, 0.587f, 0.114f), inColor);
        //greyscale = smoothstep( 0.0f, 1.0f, greyscale);
        greyscale = clamp( greyscale, 0.0f, 1.0f);
  
  return vec3( greyscale );
}


vec3 threshold( in vec3 inColor, in float factor)
{
  float avg = (inColor.r + inColor.g + inColor.b) / 3.0f;
  return vec3(step(factor, avg));
}




void main(void)
{
  vec4 sceneColor = texture( uSceneTex, vTexCoord);
  
  float ambientFactor = sceneColor.a;
  
  


  // --------------------------------------


  vec3 finalColor;

  float ao = 1.0f;
  vec3 bloomColor = vec3( 0.0f );
  
  #if 0
  if (vTexCoord.x < 0.5f) {
    finalColor = texture( uSceneTex, vTexCoord).rgb;
  } else {
    finalColor = texture( uInputTex, vTexCoord).rgb;
  }
  #else
  finalColor = sceneColor.rgb;
  #endif
  
  //
  if (uEnableAO)
  {
    ao = texture( uAOTex, vTexCoord).r;
    
    // Background elements (like sky) does not get AO
    ao = mix( 1.0f, ao, sceneColor.a);
  }

  if (uEnableBloom)
  {
    float bloom = texture( uBloomTex, vTexCoord).r;
    
    const float bloomFactor = 1.0f;
    bloomColor = finalColor * (bloomFactor * bloom); 
  }
  
  if (uEnableDoF) {
    finalColor += texture( uDoFTex, vTexCoord).rgb;
  }

  /// Compose each effects
  finalColor = ao * (finalColor + bloomColor);

  
  // --------------------------------------
  

  //finalColor = sepiaFilter( finalColor );  
  //finalColor = greyscaleFilter( finalColor );
  
  // some test
  //finalColor += finalColor / chromaticAberration();

  if (uEnableVignetting) {
    finalColor *= vignetting();
  }

  if (uEnableGammaCorrection) {
    finalColor = gammaCorrect( finalColor );
  }
  
  /*
  // XXX
  if (vTexCoord.x < 0.5f) {
    finalColor = texture( uMotionTex, vTexCoord).rgb;
  }
  /* XXX */
  
  fragColor = vec4( finalColor, 1.0f);
}


