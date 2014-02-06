/*
 *          DR_Buffering.glsl
 *
 */

//------------------------------------------------------------------------------


-- Vertex


// IN
layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// OUT
out vec3 vNormalVS;
out vec2 vTexCoord;

// UNIFORM
uniform mat4 uModelViewProjMatrix;
uniform mat3 uNormalViewMatrix;


void main()
{
  gl_Position = uModelViewProjMatrix * inPosition;
  
  // View-space normal
  vNormalVS = uNormalViewMatrix * inNormal;
    
  vTexCoord = inTexCoord;
}


--

//------------------------------------------------------------------------------


-- Fragment


// IN
in vec3 vNormalVS;
in vec2 vTexCoord;

// OUT
layout(location = 0) out vec4 fragDiffuse;
layout(location = 1) out vec3 fragNormal;



void main()
{
  // Diffuse (texture / material)
  fragDiffuse = vec4( vec3(1.0f), 1.0f);  
  
  // Shadows are set in a different pass

  // TODO Store only the XY component and retrieve the Z later
  fragNormal.rgb = (gl_FrontFacing)? vNormalVS.xyz : -vNormalVS.xyz;
}


