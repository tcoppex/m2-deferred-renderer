/*
 *          DR_Lighting.glsl
 *
 *      This is a 'GLSL 3.1+ core profile' code.
 *
 *    ref : "Reconstructing Position from depth" 
 *                           http://mynameismjp.wordpress.com/
 */

//------------------------------------------------------------------------------


-- Vertex


// IN
layout(location = 0) in vec4 inPosition;

// OUT
out vec3 vEyeDirVS;

// UNIFORM
uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uInvProjMatrix;

uniform bool uIsDirectional = false;


void directionalLight()
{
  vec2 texCoord;
  texCoord.s = (gl_VertexID << 1) & 2;
  texCoord.t = gl_VertexID & 2;

  gl_Position = vec4( 2.0f*texCoord - 1.0f, 0.0f, 1.0f);

  vEyeDirVS = vec3( 0.0f, 0.0f, 1.0f);  

  // For directionnal light (represented as a quad), we need to invert the projMatrix)
  //vec3 eyeDirVS = (uInvProjMatrix * inPosition).xyz;
  //vEyeDirVS = eyeDirVS / eyeDirVS.z;
}


void nonDirectionalLight()
{
  gl_Position = uModelViewProjMatrix * inPosition;

  // Unormalized view-Space 'view to vert' ray (will be interpolated)
  vEyeDirVS = (uModelViewMatrix * inPosition).xyz;  
}

void main()
{
  if (uIsDirectional) {
    directionalLight();
  } else {
    nonDirectionalLight();
  }
}


--

//------------------------------------------------------------------------------


-- Fragment

/*
  Note : for this version, lights must be sent in VIEW SPACE.
*/

in vec3 vEyeDirVS;


// OUT
layout(location = 0) out vec4 fragColor;

// UNIFORM
uniform sampler2D uDiffuseBuffer;
uniform sampler2D uNormalBuffer;
uniform sampler2DShadow uDepthBuffer; //

uniform vec2 uLinearParams;


vec3 computeLight( vec3 fragPos, vec3 normal );


void main()
{
  // Unpacking buffers data
  vec4 bufferData;
  vec2 texCoord = gl_FragCoord.xy / textureSize( uDiffuseBuffer, 0);

   
  bufferData = texture( uDiffuseBuffer, texCoord);
  vec3 diffuse = bufferData.rgb;

  // for unlight objects like sky
  float ambientFactor = bufferData.a;  
  
  // Depth (post-Proj space)
  float depth = texture( uDepthBuffer, vec3(texCoord,0));
    
  // linearize depth
  float linearDepth = uLinearParams.y / (depth - uLinearParams.x);
  
  
  // Position (view space)
  vec3 viewRay = (vEyeDirVS / -vEyeDirVS.z);  
  vec3 positionVS = linearDepth * viewRay;
  
  // todo: try to retrieve pos via depth + pixel coord (note : the following is wrong)
  //viewRay = vec3( normalize(2*texCoord-1), zNear) * (1.0f / zNear);
  

  // Normal (view space)
  bufferData = texture( uNormalBuffer, texCoord);

  vec3 normalVS;
  normalVS.xyz = bufferData.rgb;

  // tips: in view space, every normal are in the front hemisphere.
  // Je crois que ça ne marche qu'en 'screen space' pas en viewSpace
  // (a priori, certains Z sont négatif, ce qui ne devrait pas arriver)
  //float nZ = sqrt( 1.0f - dot(bufferData.rg, bufferData.rg) );
  //normalVS.z = nZ;
  //normalVS = normalize( normalVS );
  
  vec3 lightColor = computeLight( positionVS, normalVS );
  
  fragColor.rgb = mix( diffuse, lightColor*diffuse, ambientFactor);
  fragColor.a = ambientFactor;
  
  // Note : pour afficher correctement les buffers, il faut désactiver le blending
  //fragColor.rgb = normalVS;  
}










// =========== LIGHTS COMPUTATION =========== 

// can be set in an other file to include


#define LIGHT_POINT       0
#define LIGHT_SPOT        1
#define LIGHT_DIRECTIONAL 2

struct Attenuation
{
  float Kc;
  float Kl;
  float Kq;
};

struct Light
{
  int type;
  
  vec3 color;       // All  
  vec3 position;    // Point, Spot
  vec3 direction;   // Spot, Directionnal
  float cutoff;     // Spot
  Attenuation att;  // Point, Spot
};

uniform Light uLight;


vec3 computeBaseLight( vec3 fragPos, vec3 lightColor, vec3 lightDir, vec3 normal)
{
/**
  Actually, Ambient should be precomputed and Specular / Diffuse
  should have each a different buffer for the accumulation.
*/

  vec3 toLight = -lightDir;
  
  // Ambient Lighting
  float ambientFactor = 0.1f; //XXX
  vec3 ambientColor = ambientFactor * lightColor;
  
  // Diffuse Lighting
  float diffuseFactor = max( 0.0f, dot(normal, toLight));
  vec3 diffuseColor = diffuseFactor * lightColor;
  
  // Specular Lighting (viewspace camera = (0,0,0))
  vec3 fragToEye = normalize( - fragPos );
  vec3 reflection = normalize(reflect( lightDir, normal));
  
  float specularFactor;
  specularFactor = dot( fragToEye, reflection);
  specularFactor = max( 0.0f, specularFactor);
  specularFactor = pow( specularFactor, 125.0f);
  vec3 specularColor = specularFactor * vec3(1.0f);
  
  return ambientColor + diffuseColor + specularColor;
  
  //return vec4( ambient+diffuse, specular);  
}

vec3 computeDirectionalLight( vec3 fragPos, Light light, vec3 normal)
{
  return computeBaseLight( fragPos, light.color, -light.position, normal);
}

vec3 computePointLight( vec3 fragPos, Light light, vec3 normal)
{
  vec3 lightDir = fragPos - light.position;  
  float lenDir = length( lightDir );
  
  // normalize
  lightDir = lightDir * (1.0f / lenDir);
    
  vec3 color = computeBaseLight( fragPos, light.color, lightDir, normal);
  
  /**/

  // XXX
  if (light.type == LIGHT_POINT)
  {
    const float radius = 20.0f;//

    // Point Light Attenuation  
    float fadeout = lenDir/radius;
          fadeout = 1.0f - fadeout*fadeout;
          fadeout = smoothstep( 0.0f, 1.0f, fadeout);
    
    return color * fadeout;
  }
  /**/

  float attenuation = light.att.Kc + 
                      light.att.Kl * lenDir +
                      light.att.Kq * (lenDir * lenDir);

  return color / attenuation;
}

vec3 computeSpotLight( vec3 fragPos, Light light, vec3 normal)
{
  vec3 lightToFragment = normalize( fragPos - light.position );

  float factor = light.cutoff - abs(dot(lightToFragment, light.direction));
  factor = abs(factor);
 
  // TODO check  
    
  vec3 color = factor * computePointLight( fragPos, light, normal);  

  return color;
}

vec3 computeLight( vec3 fragPos, vec3 normal )
{
/**
  Conditionnal branches are not a good habit to take in
  shaders algorithms, but here it's quite better than having multiple
  ProgramShader.
  
  Others solutions :
    - For OpenGL 4.1+ : use subroutine
    - Otherwise : Differentiate light type.

  I'm not sure wich solution is the best or even if this one is really bad.
  (Think not).
*/

  vec3 color;
  
  // PointLight
  if (uLight.type == LIGHT_POINT)
  {
    color = computePointLight( fragPos, uLight, normal);
  }
  // Directional Light
  else if (uLight.type == LIGHT_DIRECTIONAL)
  {
    color = computeDirectionalLight( fragPos, uLight, normal);  
  }  
  // SpotLight
  else if (uLight.type == LIGHT_SPOT)
  {
    color = computeSpotLight( fragPos, uLight, normal);  
  }
    
  return color;
}


