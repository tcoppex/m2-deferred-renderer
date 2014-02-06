/*
 *          ppBloom.glsl
 *
 *
 *      version : GLSL 3.1+ core
 *
 *      This could be done in another effect's shader to improve performances.
 */

//------------------------------------------------------------------------------


-- Fragment


// IN
in vec2 vTexCoord;

// OUT
layout(location = 0) out vec4 fragColor;

// UNIFORM
uniform sampler2D uInputTex;
uniform float uThreshold = 0.15f;

void main()
{
  vec3 inColor = texture( uInputTex, vTexCoord).rgb;
  
  float avg = (inColor.r + inColor.g + inColor.b) / 3.0f;
  
  float mask = step( uThreshold, avg);
  
  fragColor = vec4( mask );
}

