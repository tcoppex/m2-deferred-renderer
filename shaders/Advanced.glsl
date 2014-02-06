/*
 *          Advanced.glsl
 *
 *    bad name, just a shader with more effects than the basic one used for
 *    the Sponza scene.
 *
 */

//------------------------------------------------------------------------------


-- Vertex


// IN
layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;

// OUT
out vec3 vNormalVS;
out vec2 vTexCoord;
out mat3 vDetailNormalMatrix;

// UNIFORM
uniform mat4 uModelViewProjMatrix;
uniform mat3 uNormalViewMatrix;


void main()
{
  gl_Position = uModelViewProjMatrix * inPosition;
  
  // View-space normal
  vNormalVS = uNormalViewMatrix * inNormal;
    
  vTexCoord = inTexCoord;


  /// Create the matrix to compute viewspace detail normals
  /// (First one trades memory for computation time)
  #if 0
  
  // matrix to compute viewspace detail normals
  vec3 tangent = cross( inNormal, vec3(1.0f, 0.0f, 0.0f));
       tangent = normalize( tangent );
  
  vec3 bitangent = cross( inNormal, tangent );
       bitangent = normalize( bitangent );

  vDetailNormalMatrix[0] = tangent;
  vDetailNormalMatrix[1] = bitangent;
  vDetailNormalMatrix[2] = inNormal;
  
  #else
    
  vDetailNormalMatrix[0] = inTangent;
  vDetailNormalMatrix[1] = inBitangent;
  vDetailNormalMatrix[2] = inNormal;
  
  #endif

  vDetailNormalMatrix = uNormalViewMatrix * vDetailNormalMatrix;
}


--

//------------------------------------------------------------------------------


-- Fragment


// IN
in vec3 vNormalVS;
in vec2 vTexCoord;
in mat3 vDetailNormalMatrix;

// OUT
layout(location = 0) out vec4 fragDiffuse;
layout(location = 1) out vec3 fragNormal;

// UNIFORM
uniform sampler2D uDiffuseTex;
uniform sampler2D uNormalTex;
//uniform vec3 uKd;
//uniform vec3 uKs;
uniform bool uBumpMapping = false;


void main()
{
  vec4 albedo = texture( uDiffuseTex, vTexCoord);  
  
  // basic alpha testing
  if (albedo.a < 0.5f) discard;
  
  // Diffuse (texture / material)
  fragDiffuse = vec4( albedo.rgb, 1.0f);
  
  
  vec3 normal;
    
  if (uBumpMapping)
  {
    vec3 detailNormal = 2.0f * texture( uNormalTex, vTexCoord).xyz - 1.0f;
         detailNormal = vDetailNormalMatrix * detailNormal;
    
    normal = normalize( detailNormal );
  }
  else
  {
    normal = vNormalVS;
  }
  
  // Store only the XY component and retrieve the Z later (TODO, some bug with light on border)
  fragNormal.rgb = normal;//(gl_FrontFacing)? normal : -normal;
}



--

//------------------------------------------------------------------------------


-- LoD.Vertex

// IN
layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// OUT
out vec3 vNormalVS;
out vec4 vColor;

// UNIFORM
uniform mat4 uModelViewProjMatrix;
uniform mat3 uNormalViewMatrix;
uniform sampler2D uDiffuseTex;

void main()
{
  gl_Position = uModelViewProjMatrix * inPosition;
  
  // View-space normal
  vNormalVS = uNormalViewMatrix * inNormal;
  
  vColor = texture( uDiffuseTex, inTexCoord);
}


--

//------------------------------------------------------------------------------


-- LoD.Fragment


// IN
in vec3 vNormalVS;
in vec4 vColor;

// OUT
layout(location = 0) out vec4 fragDiffuse;
layout(location = 1) out vec3 fragNormal;

void main()
{
  if (vColor.a < 0.5f) discard;
  
  // Diffuse (texture / material)
  fragDiffuse = vec4( vColor.rgb, 1.0f);
  
  fragNormal.rgb = (gl_FrontFacing)? vNormalVS : -vNormalVS;
}


