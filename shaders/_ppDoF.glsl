/*
 *          ppDoF.glsl
 *
 */

//------------------------------------------------------------------------------


-- Fragment


// IN
in vec2 vTexCoord;

// OUT
layout(location = 0) out vec4 fragColor;

// UNIFORM
uniform sampler2D uSceneTex;
uniform sampler2D uDepthTex;

uniform vec2 uKernels[12];
uniform int uNumSamples = 12;

uniform vec2 uLinearParams;


float getLinearDepth( in vec2 uv )
{
  float depth = texture( uDepthTex, uv).r;
  return uLinearParams.y / (depth - uLinearParams.x);
}

void main()
{
  // Retrieve the fragment's color
  vec3 inColor = texture( uSceneTex, vTexCoord).rgb;

  // Retrieve the fragment's depth (in linear view space)
  float depth = getLinearDepth( vTexCoord );
  
  vec2 texelSize = 1.0f/textureSize( uSceneTex, 0); // XXX
  
  vec3 blurColor = vec3(0.0f);

  float blurFactor = depth * 0.4f;
        blurFactor = min( blurFactor*blurFactor, 12.0f/13.0f);
  
  for (int i=0; i<uNumSamples; ++i)
  {
    vec2 uv = vTexCoord + uKernels[i]*texelSize;
    
    vec3 sampleColor = texture( uSceneTex, uv).rgb; 
    float sampleDepth = getLinearDepth( uv );
    
    float sampleBlurFactor = sampleDepth * 0.4f;
          sampleBlurFactor = min( sampleBlurFactor*sampleBlurFactor, 12.0f/13.0f);

    float factor = sampleDepth;
    
    blurColor += mix( inColor, sampleColor, factor);
  }
  blurColor /= uNumSamples;
  
  fragColor = vec4( blurColor, 1.0f);
}

