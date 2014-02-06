/**
 *             
 *                \file LightManager.hpp
 * 
 */
 
#ifndef GRAPHIC_LIGHTMANAGER_HPP
#define GRAPHIC_LIGHTMANAGER_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <list>
#include "Mesh.hpp"
#include "Light.hpp"

class Camera;


enum LightRenderMode
{
  LRM_GEOMETRY = 0,
  LRM_DEFERREDPASS,
  
  NUM_LIGHTRENDERMODE
};


class LightManager
{
  private:
    /** Every lights of the scene */
    std::list<Light*> m_SceneLights;    
    //std::list<Light*> m_castingShadowsLights;
    
    /** [optionnal] Pointer to the main directionnal light (by default the first dirlight sent) */
    DirectionalLight *m_pSun;
    
    /** [optionnal] Pointer to lights used in forward / blend passes   */
    static const size_t MAX_FORWARD_LIGHT = 8u;
    Light *m_pForwardLight[MAX_FORWARD_LIGHT];
    size_t m_numForwardLights;
    
    /** Canonical light shape */
    Mesh* m_meshes[NUM_LIGHTTYPE]; // use InstancedModel instead
    

    /** [optionnal] */
    //float m_ambientFactor;
    
    /** Use to count the number of pixels 'lighted' during the deferred light pass.
     * This information can be used to know if the shadow for a given light must
     * be created or not */
    GLuint m_queryLightSamples; //TODO: encapsulate
    
  
  public:
    LightManager()
      : m_pSun(0),
        m_numForwardLights(0u),
        m_queryLightSamples(0u)
    {}
    ~LightManager();
    
    void init();
    
    void update();
    
    /** Deferred light pass or render geometry */
    void render(LightRenderMode mode, const Camera& camera);
    
    /** Set uniforms for forward lights */
    void setForwardLightUniforms(const Camera& camera);
        
    int addLight( Light *light );    
    //void removeLight(int id);    
    //Light* getLight(int id);
    
    void clearAll();
        
    
  private:
    void _deferredLightPass(const Camera& camera);
    void _renderGeometry(const Camera& camera);    
    void _drawLightshape(Light* pLight);
};

#endif //GRAPHIC_LIGHTMANAGER_HPP
