/**
 *             
 *                \file Light.hpp
 * 
 *        \note SpotLight could also have been derived from PointLight
 * 
 *        \todo find a (clever) way to attach lights to moving objects
 * 
 */

#ifndef GRAPHIC_LIGHT_HPP
#define GRAPHIC_LIGHT_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <cassert>

class Camera;


enum LightType
{  
  LIGHT_POINT = 0,  
  LIGHT_SPOT,
  LIGHT_DIRECTIONAL,
  //LIGHT_AMBIENT,  // ¿ include in DIRECTIONAL or statically in LightManager ?
  
  NUM_LIGHTTYPE
};


struct Light
{
  public:
    bool bEnable;                 /*! Light is use only if enabled */
    bool bForwardLight;           /*! If true, used in forward passes */
    
    glm::vec3 position;           /*! A vector for directional, a point otherwise */
    glm::vec3 color;              /*! RGB intensity */
    

  public:
    Light(const glm::vec3& v3Color, const glm::vec3& v3Position) 
      : bEnable(true), 
        bForwardLight(false),        
        position(v3Position),
        color(v3Color)
    {}
    
    virtual ~Light() {}
    
    /** Return true if a point lies in the light shape, false otherwise */
    virtual bool intersectPoint(const glm::vec3& pt) = 0;
    
    /** Set lights' uniforms for a deferred pipeline */
    virtual void setDeferredUniforms(const Camera& camera) = 0;
    
    /** Set lights' uniforms for a forward pipeline.
     * As they are usually sent as an array of lights, a buffer containing the
     * element name & a pointer to the end of this buffer are sent to append
     * the uniform's name.*/
    virtual void setForwardUniforms(const Camera& camera, char *buffer, char *pStart) = 0;
    
    /** Return the type of the light */
    virtual LightType getType() = 0;
};



struct DirectionalLight : public Light
{
  public:
    explicit
    DirectionalLight( const glm::vec3& v3Color, 
                      const glm::vec3& v3Position);
        
    bool intersectPoint(const glm::vec3& pt)  {return true;}
    
    void setDeferredUniforms(const Camera& camera);
    void setForwardUniforms(const Camera& camera, char *buffer, char *pStart);
    
    LightType getType() {return LIGHT_DIRECTIONAL;}
};


struct PointLight : public Light
{
  public:
    glm::mat4 shapeMatrix;  /*! light shape model matrix */    
    float radius;           /*! lightshape radius */
    
    struct {
      float Kc;
      float Kl;
      float Kq;
    } attenuation;  
  
  
  public:
    explicit
    PointLight( const glm::vec3& v3Color, 
                const glm::vec3& v3Position, 
                float fRadius=10.0f);  
    
    
    bool intersectPoint(const glm::vec3& pt);
    
    void setDeferredUniforms(const Camera& camera);
    void setForwardUniforms(const Camera& camera, char *buffer, char *pStart);
    
    LightType getType() {return LIGHT_POINT;}
};


struct SpotLight : public Light
{
  public:
    glm::vec3 direction;    
    glm::mat4 shapeMatrix;    /*! light shape */
    float height;             /*! radius for PointLight, cone's height for spotlight */
    float cutoff;             /*! value in [0.0, 1.0] where 0.0 is wide open, 1.0 is closed */
  
    struct {
      float Kc;
      float Kl;
      float Kq;
    } attenuation;  

  public:
    explicit
    SpotLight( const glm::vec3& v3Color, 
               const glm::vec3& v3Position, 
               const glm::vec3& v3Direction, 
               float fHeight = 100.0f, 
               float fCutoff = 0.5f );
      
    
    bool intersectPoint(const glm::vec3& pt);
    
    void setDeferredUniforms(const Camera& camera);
    void setForwardUniforms(const Camera& camera, char *buffer, char *pStart);
    
    LightType getType() {return LIGHT_SPOT;}
};


#endif //GRAPHIC_LIGHT_HPP
