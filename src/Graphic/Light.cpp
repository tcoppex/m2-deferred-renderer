/**
 *             
 *                \file Light.cpp
 * 
 * 
 */

#include "Light.hpp"

#include <GLType/ProgramShader.hpp>
#include <tools/Camera.hpp>
#include <tools/gltools.hpp>



/** DIRECTIONAL LIGHT --------------------------------------------------------*/

DirectionalLight::DirectionalLight( const glm::vec3& v3Color, 
                                    const glm::vec3& v3Position)
  : Light( v3Color, glm::normalize(v3Position))
{
}

void DirectionalLight::setDeferredUniforms(const Camera& camera)
{
  /**
   * The code for the Directionnal light is different from the others, directionnal
   * light being non local, we use a 2D quad mapping the screen corner to represent
   * it. A different vertex code is thus also used (a different branch).
   * Actually, we should be able to represent directionnal light with a frustum
   * shape using the invert of the projection matrix, but tests are not concluent
   * so far.
   */

  const ProgramShader& program = ProgramShader::GetCurrent();  
    
  program.setUniform( "uInvProjMatrix", camera.getInvProjectionMatrix());
  
  program.setUniform( "uIsDirectional", true);
  program.setUniform( "uLight.type", LIGHT_DIRECTIONAL);  
  program.setUniform( "uLight.color", color);
  
  /// Here the source light is at an infite point, thus position is a vector.
  const glm::mat3 modelView = glm::mat3(camera.getViewMatrix());
  const glm::vec3& positionVS = modelView * position;
  program.setUniform( "uLight.position", positionVS);
  
  CHECKGLERROR();
}

void DirectionalLight::setForwardUniforms( const Camera& camera, 
                                           char *buffer, 
                                           char *pStart)
{
  const ProgramShader& program = ProgramShader::GetCurrent();  
  
  sprintf( pStart, "type");
  program.setUniform( buffer, LIGHT_DIRECTIONAL);  
  
  sprintf( pStart, "color");
  program.setUniform( buffer, color);
  
  /// the position is sent in world space
  sprintf( pStart, "position");
  program.setUniform( buffer, position);
  
  CHECKGLERROR();
}




/** POINT LIGHT --------------------------------------------------------------*/


PointLight::PointLight( const glm::vec3& v3Color, 
                        const glm::vec3& v3Position, 
                        float fRadius)
  : Light( v3Color, v3Position),
    radius(fRadius)
{
  shapeMatrix = glm::scale( glm::mat4(1.0f), glm::vec3(radius));
  
  attenuation.Kc = 1.00f;
  attenuation.Kl = 0.01f;
  attenuation.Kq = 0.00f;
}

bool PointLight::intersectPoint(const glm::vec3& pt)
{      
  const glm::vec3 &v = pt - position;
  
  /// the radius is slightly increased to avoid boundaries problems.
  float rsqr = (radius*radius)*1.125f; // can be precomputed most of the time
  return glm::dot(v,v) < rsqr;
}  

void PointLight::setDeferredUniforms(const Camera& camera)
{
  const ProgramShader& program = ProgramShader::GetCurrent();  
  
  const glm::mat4& model = glm::translate( glm::mat4(1.0f), position) * shapeMatrix;
    
  const glm::mat4& mvpMatrix = camera.getViewProjMatrix() * model;
  program.setUniform( "uModelViewProjMatrix", mvpMatrix);
  
  const glm::mat4& modelView = camera.getViewMatrix() * model;
  program.setUniform( "uModelViewMatrix", modelView);//
    
  program.setUniform( "uIsDirectional", false);
  
  program.setUniform( "uLight.type", LIGHT_POINT);  
  program.setUniform( "uLight.color", color);
    
  const glm::vec3& positionVS = glm::vec3(modelView[3]);    
  program.setUniform( "uLight.position", positionVS);
    
  program.setUniform( "uLight.att.Kc", attenuation.Kc);
  program.setUniform( "uLight.att.Kl", attenuation.Kl);
  program.setUniform( "uLight.att.Kq", attenuation.Kq);
  
  CHECKGLERROR();
}

void PointLight::setForwardUniforms( const Camera& camera, 
                                     char *buffer, 
                                     char *pStart)
{
  const ProgramShader& program = ProgramShader::GetCurrent();  
  
  sprintf( pStart, "type");
  program.setUniform( buffer, LIGHT_POINT);  
  
  sprintf( pStart, "color");
  program.setUniform( buffer, color);
  
  /// the position is sent in world space
  sprintf( pStart, "position");
  program.setUniform( buffer, position);
  
  sprintf( pStart, "att.Kc");
  program.setUniform( buffer, attenuation.Kc);
  
  sprintf( pStart, "att.Kl");
  program.setUniform( buffer, attenuation.Kl);
  
  sprintf( pStart, "att.Kq");
  program.setUniform( buffer, attenuation.Kq);
  
  CHECKGLERROR();
}




/** SPOT LIGHT ---------------------------------------------------------------*/


SpotLight::SpotLight( const glm::vec3& v3Color, 
                      const glm::vec3& v3Position, 
                      const glm::vec3& v3Direction, 
                      float fHeight,
                      float fCutoff)
  : Light(v3Color, v3Position)
{
  direction = glm::normalize(v3Direction);
  height = glm::max( 1.0f, fHeight);
  cutoff = glm::clamp( fCutoff, 0.0f, 1.0f-FLT_EPSILON);
  
  // something is wrong here, don't know what..
  double alpha = M_PI_2 * (1.0f - cutoff);  
  double scaleZ = height;
  double scaleXY = scaleZ * glm::tan(alpha);
  
  shapeMatrix = glm::scale( glm::mat4(1.0f), glm::vec3( scaleXY, scaleXY, scaleZ));
  
  attenuation.Kc = 1.00f;
  attenuation.Kl = 0.01f;
  attenuation.Kq = 0.00f;
}  


bool SpotLight::intersectPoint(const glm::vec3& pt)
{
  /*
  //------- A REVOIR (mais detection à priori OK)
  
  glm::vec3 v = pt - position;
  const glm::vec3 n = glm::normalize(v);
  
  float dp = glm::dot( n, direction);  
  float lenProjView = glm::dot(v, direction);
  
  //printf( "%f %f %f / %.2f %.2f %.2f\n", dp, cutoff, glm::length(v), pt.x, pt.y, pt.z);
  
  bool r = (dp > cutoff) &&
            // True if the camera lies beetween the two parallel planes containing the cone
           (lenProjView >= 0.0f) && (lenProjView <= height);
  
  // DEBUG color
  //color = (r)?glm::vec3(1.0f, 0.0f, 0.0f):glm::vec3(0.0f, 1.0f, 0.0f);
  
  return r;
  */
  
  return false;
}

  
void SpotLight::setDeferredUniforms(const Camera& camera)
{ 
  const ProgramShader &program = ProgramShader::GetCurrent();
   
 
  // 1) Transformation
  /*static*/
  const glm::vec3 baseOrientation( 0.0f, 0.0f, -1.0f);
  
  
  glm::vec3 rAxis = glm::cross( baseOrientation, direction);
  
  /* |rAxis|² is either 1 or 0, (0 <=> baseOrientation == +-direction) */
  if (glm::dot(rAxis, rAxis) < FLT_EPSILON) {
    rAxis = glm::vec3( 0.0f, 1.0f, 0.0f);
  }
  
  float dp = glm::dot( baseOrientation, direction);
  float alpha = (1.0f - dp) * 90.0f; // => [0; 180]
  
  // Position / Orientation
  glm::mat4 model;
  model = glm::translate( glm::mat4(1.0f), position);
  model = glm::rotate( model, alpha, rAxis); //
  
  // 2) Shape scaling
  model = model * shapeMatrix; 
  
  
  const glm::mat4& mvpMatrix = camera.getViewProjMatrix() * model;
  program.setUniform( "uModelViewProjMatrix", mvpMatrix);
  
  const glm::mat4& view = camera.getViewMatrix();
  const glm::mat4& modelView = view * model;
  program.setUniform( "uModelViewMatrix", modelView);


  program.setUniform( "uIsDirectional", false);
  program.setUniform( "uLight.type", LIGHT_SPOT);
  program.setUniform( "uLight.color", color);
      
  const glm::vec3& positionVS = glm::vec3( modelView[3] );
  program.setUniform( "uLight.position", positionVS);
  
  const glm::vec3& directionVS = glm::mat3(view) * direction;   //
  program.setUniform( "uLight.direction", directionVS);       
     
  program.setUniform( "uLight.cutoff", cutoff);
    
  program.setUniform( "uLight.att.Kc", attenuation.Kc);
  program.setUniform( "uLight.att.Kl", attenuation.Kl);
  program.setUniform( "uLight.att.Kq", attenuation.Kq);
  
  
  CHECKGLERROR();
}

void SpotLight::setForwardUniforms( const Camera& camera, 
                                    char *buffer, 
                                    char *pStart)
{
  const ProgramShader& program = ProgramShader::GetCurrent();  
  
  sprintf( pStart, "type");
  program.setUniform( buffer, LIGHT_SPOT);  
  
  sprintf( pStart, "color");
  program.setUniform( buffer, color);
  
  /// the position is sent in world space
  sprintf( pStart, "position");
  program.setUniform( buffer, position);
  
  /// as for the direction
  sprintf( pStart, "direction");
  program.setUniform( buffer, direction);
  
  sprintf( pStart, "cutoff");
  program.setUniform( buffer, cutoff);
  
  sprintf( pStart, "att.Kc");
  program.setUniform( buffer, attenuation.Kc);
  
  sprintf( pStart, "att.Kl");
  program.setUniform( buffer, attenuation.Kl);
  
  sprintf( pStart, "att.Kq");
  program.setUniform( buffer, attenuation.Kq);
  
  CHECKGLERROR();
}
