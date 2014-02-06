/**
 * 
 *      \file Scene.hpp
 * 
 *  A scene object contains geometry & lights to be rendered.
 * 
 *    \todo : rewrite completely
 *            its (future) purpose is to act like a scene graph,
 *            to load scene from file.
 * 
 */


#pragma once

#ifndef SCENE_SCENE_HPP
#define SCENE_SCENE_HPP

#include <Graphic/LightManager.hpp>
#include <GL/glew.h>
#include <vector>

class Camera;
class Model;
class SkyDome;
class SponzaModel;
class WaterPlane;
class SineDragon;
class CubeSea;


enum SceneRenderMode
{
  RENDER_GEOMETRY = 0,
  RENDER_LIGHTGEOMETRY,
  RENDER_LIGHTSHAPE,
  
  NUM_SCENERENDERMODE
};

class Scene
{
  protected:
    bool m_bInitialized;
    
    LightManager m_lightManager;
    
    //ModelManager m_modelManager;        
    std::vector<Model*> m_vModels;//
    std::vector<Model*> m_vTransparentMeshes;//
    
    SkyDome *m_SkyDome;
    SponzaModel *m_Sponza;
    WaterPlane *m_WaterPlane;
    SineDragon *m_SineDragon;
    CubeSea *m_CubeSea;
    
    Camera &m_rCamera;
    
        
  public:
    Scene();
    ~Scene();
    
    void init();
    
    void update();
    
    void render(SceneRenderMode type);
    

  protected:
    void _initGeometry();
    void _initLights();
            
    void _renderModels();
    
    void _preprocess();
};


#endif //SCENE_SCENE_HPP
