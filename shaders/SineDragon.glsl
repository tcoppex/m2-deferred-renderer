/*
 *          SineDragon.glsl
 *
 *      Program shader for the sine dragon, using instanced geometry.
 *
 */

//------------------------------------------------------------------------------


-- Vertex


// IN
layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
// 3 & 4 are reserved for tangent & bitangent.. change this later
layout(location = 5) in mat4 inModelMatrix;
layout(location = 9) in mat3 inNormalMatrix;

// OUT
out vec3 vNormal;
flat out int vInstanceID;

// UNIFORM
uniform mat4 uViewProjMatrix;
uniform mat4 uViewMatrix;


void main()
{
  gl_Position = uViewProjMatrix * inModelMatrix * inPosition;  

  // WorldSpace Normal
  vNormal = mat3(uViewMatrix) * inNormalMatrix * inPosition.xyz;
  //vNormal = normalize(vNormal);
  
  vInstanceID = gl_InstanceID;
}


--

//------------------------------------------------------------------------------


-- Fragment


// IN
in vec3 vNormal;
flat in int vInstanceID;

// OUT
//Deferred G-Buffers
layout(location = 0) out vec4 fragDiffuse;
layout(location = 1) out vec3 fragNormal;

// UNIFORM
uniform int uNumInstance;

void main()
{
  float c = vInstanceID / float(uNumInstance);
  
  fragDiffuse = vec4( vec3( c, 0.0f, 0.0f), 1.0f);

  fragNormal.rgb = vNormal.xyz;
}



--

//------------------------------------------------------------------------------


-- Debug.Fragment


// IN
in vec3 vNormal;
flat in int vInstanceID;

// OUT
//Deferred G-Buffers
layout(location = 0) out vec4 fragDiffuse;
layout(location = 1) out vec3 fragNormal;

// UNIFORM
uniform int uNumInstance;

void main()
{
  float c = vInstanceID / float(uNumInstance);
  
  fragDiffuse = vec4( 1.0f );
  //fragDiffuse = vec4( vec3( c, 0.0f, 0.0f), 1.0f);

  fragNormal.rgb = vNormal.xyz;
}

