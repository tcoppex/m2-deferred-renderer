/*
 *          ppBlur.glsl
 *
 *
 *        Differents fragments program to blur an input texture.
 *
 *        'Fragment' makes horizontal or vertical RGBA blurs.
 *
 *        'Depth.Fragment' makes horizontal or vertical R blur with depth's
 *        information.
 */


//------------------------------------------------------------------------------


-- Fragment

// IN
in vec2 vTexCoord;

// OUT
layout(location = 0) out vec4 fragColor;

// UNIFORM
uniform sampler2D uInputTex;

uniform bool uHorizontal;
uniform int uKernelSize = 8;


// ? blur alpha channel too ?
vec4 linearBlur( const in vec2 blurVector, 
                 const in int halfSize )
{
  vec4 blurColor = vec4(0.0f);
  int sum_weight = 0;

  for (int i=-halfSize; i<=halfSize; ++i)
  {
	  vec4 value = texture( uInputTex, vTexCoord + i * blurVector);

    // weight, function of sample distance
	  int w = halfSize - abs(i) + 1;
	  
	  blurColor += w * value;	  
	  sum_weight += w;
  }  
  blurColor /= sum_weight;
  
  return blurColor;
}


void main()
{
  vec2 texelSize = 1.0f / textureSize( uInputTex, 0); //set as uniform ?  //XXX
  vec4 blurColor;
  
  if (uHorizontal)
  {
    blurColor = linearBlur( vec2(texelSize.x, 0.0f), uKernelSize);
  }
  else
  {
    blurColor = linearBlur( vec2(0.0f, texelSize.y), uKernelSize);
  }
  
  fragColor = blurColor;
  //fragColor.a = 1.0f;
}


--

//------------------------------------------------------------------------------


-- Depth.Fragment


// IN
in vec2 vTexCoord;

// OUT
layout(location = 0) out vec4 fragColor;

// UNIFORM
uniform sampler2D uInputTex;
uniform sampler2D uDepthTex;

uniform bool uHorizontal;
uniform float uFalloff = 0.08f;
uniform float uSharpness = 10000.0f;
uniform int uKernelSize = 8;

uniform vec2 uLinearParams;//


float getLinearDepth(in vec2 uv)
{
  // other solution : keep linearize depth in a buffer components..
  // it use memory, but it's cheaper than recomputing it for every effects
  float depth = texture( uDepthTex, uv).r;
  return uLinearParams.y / (depth - uLinearParams.x);
}


float linearBlur( const in vec2 blurVector, 
                  const in int halfSize )
{
  float blurColor = 0.0f;
  float sum_weight = 0.0f;

  float depth = getLinearDepth(vTexCoord);
  
  for (int i=-halfSize; i<=halfSize; ++i)
  {
    vec2 uv = vTexCoord + i * blurVector;
    
	  float value = texture( uInputTex, uv).r;
    float diff = getLinearDepth( uv ) - depth;
    
    // weight, function of distance & some parameters
	  float w = exp( - (i*i*uFalloff + diff*diff*uSharpness) );
	  
	  blurColor += w * value;
	  sum_weight += w;
  }
  blurColor /= sum_weight;
  
  return blurColor;
}

void main()
{
  vec2 texelSize = 1.0f / textureSize( uInputTex, 0); //set as uniform ? //XXX
  float blurColor;
  
  if (uHorizontal)
  {
    blurColor = linearBlur( vec2(texelSize.x, 0.0f), uKernelSize/2);
  }
  else
  {
    blurColor = linearBlur( vec2(0.0f, texelSize.y), uKernelSize/2);
  }
  
  fragColor = vec4( blurColor, 0.0f, 0.0f, 1.0f);
}



--

//------------------------------------------------------------------------------


#if 0

-- Simplest.Fragment

// IN
in vec2 vTexCoord;

// OUT
layout(location = 0) out vec4 fragColor;

// UNIFORM
uniform sampler2D uInputTex;


void main()
{
  vec2 texelSize = 1.0f / textureSize( uInputTex, 0);
  vec3 uv = vec3( texelSize, 0.0f);
  
  vec4 color = texture( uInputTex, vTexCoord) +
               texture( uInputTex, vTexCoord + uv.xz) +
               texture( uInputTex, vTexCoord + uv.yz) +
               texture( uInputTex, vTexCoord - uv.yz) +
               texture( uInputTex, vTexCoord - uv.xz);
               
  fragColor = color / 4.0f;
}

#endif
