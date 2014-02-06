/*
 *          PassThrough.glsl
 *
 */

//------------------------------------------------------------------------------


-- Vertex


// IN
layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// OUT
out vec3 vNormal;

// UNIFORM
uniform mat4 uModelViewProjMatrix;
uniform mat3 uNormalMatrix;


void main()
{
  gl_Position = uModelViewProjMatrix * inPosition;

  // Compute World-Space normal
  //vNormal = uNormalMatrix * inNormal;
}


--

//------------------------------------------------------------------------------


-- Fragment


// IN
in vec3 vNormal;

// OUT
layout(location = 0) out vec4 fragColor;

// UNIFORM
uniform vec4 uColor = vec4( 1.0f );


void main()
{
  fragColor = uColor;
}

