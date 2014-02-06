/*
 *          ppSSAO.glsl
 *
 */

//------------------------------------------------------------------------------


-- Fragment


// IN
in vec2 vTexCoord;

// OUT
layout(location = 0) out vec4 fragColor;

// UNIFORMs
uniform sampler2D uDepthTex;
uniform sampler2D uNormalTex;
uniform sampler2D uRandTex;

uniform vec3 uSpherePoints[32];
uniform int uNumSample = 8;
uniform float uFalloff = 0.0000005f;
uniform float uRadius = 1.0f/400.0f;
uniform float uFactor = 8.0f;

uniform vec2 uRandTexelSize;
uniform vec2 uLinearParams;//


float getLinearDepth( in vec2 uv )
{
  float depth = texture( uDepthTex, uv).r;
  return uLinearParams.y / (depth - uLinearParams.x);
}

vec3 getNormal( in vec2 uv )
{
  return texture( uNormalTex, vTexCoord).xyz;
}


/**
  Casts numSample rays in the fragment hemisphere (toward its normal)
  & update the ambientOcclusion term when their intersects occluders.
*/
void castRay( in vec3 random,
              in vec3 position,
              in vec3 normal,
              in int numSample,
              in float radius,
              inout float ambientOcclusion )
{
  float ao = 0.0f;
  
  for(int i=0; i<numSample; ++i)
  {
    vec3 ray = radius * reflect( uSpherePoints[i], random);
         ray *= sign(dot(ray,normal));

    vec3 samplePosition = position + ray;
    float sampleDepth = getLinearDepth( samplePosition.xy );

    // test whereas fragment is behind an occluder or not
    float depthDiff = position.z - sampleDepth;

    // calculate the difference between the normals as a weight
    vec3 sampleNormal = texture( uNormalTex, samplePosition.st).xyz;
    float normalDiff = 1.0f - dot( sampleNormal, normal);
  
    float occ = normalDiff * step( uFalloff, depthDiff);
          occ *= 1.0f - smoothstep( uFalloff, 1.5f,  depthDiff);

    ao += occ;
  }
  
  ambientOcclusion = ao / numSample;
}


void main(void)
{
  vec2 uv = vTexCoord * uRandTexelSize;

  // Per Fragment noise vector
  vec3 random = texture( uRandTex, uv).xyz;
       random = 2.0f * random - 1.0f;

  vec3 normal = getNormal( vTexCoord );

  // fragment depth
  float fragDepth = getLinearDepth( vTexCoord );

  // Screen-space fragment position
  vec3 position = vec3( vTexCoord.xy, fragDepth);


  float ao = 0.0f;

  #if 1
    castRay( random, position, normal, uNumSample, uRadius, ao);
  #else
    castRay( random, position, normal, uNumSample/2, uRadius, ao);
    castRay( random, position, normal, uNumSample/2, 2.0f*uRadius, ao);
  #endif

  ao *= uFactor;
  ao = 1.0f - ao;
  ao = clamp( ao, 0.0f, 1.0f);     
  
  fragColor = vec4( ao, 0.0f, 0.0f, 1.0f);
}

