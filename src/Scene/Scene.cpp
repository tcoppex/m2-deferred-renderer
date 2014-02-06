/**
 * 
 *      \file Scene.cpp
 * 
 */

#include "Scene.hpp"
#include "SkyDome.hpp"
#include "SponzaModel.hpp"
#include "WaterPlane.hpp"
#include "SineDragon.hpp"
#include "CubeSea.hpp"

#include <iostream>
#include <tools/Camera.hpp>
#include <tools/gltools.hpp>
#include <tools/AppConfig.hpp>
#include <GLType/ProgramShader.hpp>
#include <Graphic/Model.hpp>
#include <Graphic/Mesh.hpp>
#include <Graphic/Light.hpp>
#include <glm/gtc/random.hpp>



Scene::Scene()
  : m_bInitialized(false),
    m_SkyDome(0),
    m_Sponza(0),
    m_WaterPlane(0),
    m_rCamera( Camera::GetInstance() )
{  
  m_vModels.clear();
}

Scene::~Scene()
{
  if (!m_vModels.empty())
  {
    for (size_t i=0; i<m_vModels.size(); ++i) {
      delete m_vModels[i];
    }
    m_vModels.clear();
  }
  
  if (m_SkyDome) delete m_SkyDome;
  if (m_Sponza) delete m_Sponza;
  if (m_WaterPlane) delete m_WaterPlane;
  if (m_SineDragon) delete m_SineDragon;
  if (m_CubeSea) delete m_CubeSea;
}


void Scene::init()
{
  _initGeometry();
  _initLights();
  
  
  m_SkyDome = new SkyDome();
  m_SkyDome->init();
  
  m_Sponza = new SponzaModel();
  m_Sponza->init();
  
  m_WaterPlane = new WaterPlane();
  m_WaterPlane->init();
    
  m_SineDragon = new SineDragon();
  m_SineDragon->init();
  
  m_CubeSea = new CubeSea();
  m_CubeSea->init();
  
  m_bInitialized = true;
}

void Scene::update()
{
  m_lightManager.update();
  
  m_SineDragon->update();
  
  _preprocess();
}

void Scene::_preprocess()
{
  // QUITE BAD !!
  
  /// Water reflection  
  #if 1
  //if (AppConfig::GetInstance().getBool( "bWaterReflection" ))
  {    
    m_WaterPlane->beginReflection( m_rCamera );
     // _renderModels();
      m_SkyDome->render( m_rCamera );
      m_SineDragon->render( m_rCamera );
    m_WaterPlane->endReflection( m_rCamera );
  }  
  #endif
}

void Scene::render( SceneRenderMode type )
{
  assert( m_bInitialized );
  
  switch (type)
  {
    case RENDER_GEOMETRY:
      _renderModels();
    break;
        
    case RENDER_LIGHTGEOMETRY:
      m_lightManager.render( LRM_GEOMETRY, m_rCamera);
    break;
    
    case RENDER_LIGHTSHAPE:
      m_lightManager.render( LRM_DEFERREDPASS, m_rCamera);
    break;
    
    default:
      assert( "Scene::render: invalid switch to default." && 0 );
    break;    
  }
  
  CHECKGLERROR();
}

void Scene::_renderModels()
{ 
  const ProgramShader& program = ProgramShader::GetCurrent();  
  
  for (size_t i=0; i<m_vModels.size(); ++i)
  {
    Model *model = m_vModels[i];
        
    const glm::mat4& mvpMatrix = m_rCamera.getViewProjMatrix() * model->getWorldMatrix();
    program.setUniform( "uModelViewProjMatrix", mvpMatrix);
        
    const glm::mat3& normalView = glm::mat3( m_rCamera.getViewMatrix() ) * model->getNormalMatrix();
    program.setUniform( "uNormalViewMatrix", normalView);
    
    model->draw();
  }

  
  m_SkyDome->render( m_rCamera );
  
  
  
  if (AppConfig::GetInstance().getBool( "Water:Enable" )) {
    m_WaterPlane->render( m_rCamera );
  } else {
    m_Sponza->render( m_rCamera );
  }
  
  
  m_SineDragon->render( m_rCamera );
  
  //m_CubeSea->render( m_rCamera );
  
  
  CHECKGLERROR();
}


void Scene::_initGeometry()
{
  
  CHECKGLERROR();
}

void Scene::_initLights()
{  
  m_lightManager.init();
  
  
  glm::vec3 position;
  glm::vec3 direction;
  glm::vec3 color;  
  
  // DirectionalLight
  color = glm::vec3(1.0f);
  position = glm::normalize( glm::vec3( 1.0f, 5.0f, 5.0f) );
  m_lightManager.addLight( new DirectionalLight( color, position ) );
  
  
  glm::vec3 cornerA( 18.0f, 2.0f, 6.5f);
  glm::vec3 cornerB( -18.0f, 2.0f, 6.5f);
  
  glm::vec3 cornerC( 18.0f, 15.6f, -3.0f);
  glm::vec3 cornerD( -18.0f, 15.6f, -3.0f);
  
  
  /**/
  
  /// PointLight
  
  const int num=10;
  for (int i=0; i<num; ++i)
  {
    float di = i / float(num);
    
    color = glm::linearRand( glm::vec3(0.0f), glm::vec3(0.5f));
    position = glm::mix( cornerA, cornerB, di);
    float radius = glm::linearRand( 3.0f, 4.5f);
    m_lightManager.addLight( new PointLight( color, position, radius) );
    
    color = glm::linearRand( glm::vec3(0.0f), glm::vec3(0.9f));
    position = glm::mix( cornerC, cornerD, di);
    radius = glm::linearRand( 2.0f, 4.0f);
    m_lightManager.addLight( new PointLight( color, position, radius) );
  }
  
  
  color = glm::vec3( 0.75f, 0.0f, 0.0f);
  position = glm::vec3( -14.76f, 19.64f, 2.25f);
  m_lightManager.addLight( new PointLight( color, position, 15.0f) );
  
  color = glm::vec3( 0.50f, 0.30f, 0.0f);
  position = glm::vec3( 13.5f, 3.5f, -8.4f);
  m_lightManager.addLight( new PointLight( color, position, 15.0f) );
  
  
  
  /**/
  
  /// SpotLight  
  color = glm::vec3( 0.5f, 0.5f, 1.0f);
  position = glm::vec3( 0.6f, 18.0f, 1.8f);
  direction = glm::normalize(glm::vec3( 11.1f, 0.1f, -4.8f));
  m_lightManager.addLight( new SpotLight( color, position, direction, 20.0f, 0.65f) );  
  
  /*
  color = glm::vec3( 0.0f, 1.0f, 0.0f);
  position = glm::vec3( 20.0f, 10.0f, 0.0f);
  direction = glm::normalize( -position );
  m_lightManager.addLight( new SpotLight( color, position, direction, 80.0f, 0.65f) );  
  
  color = glm::vec3( 0.2f, 0.75f, 1.0f);
  position = glm::vec3( -20.0f, 20.0f, 10.0f);
  direction = glm::normalize( -position );
  m_lightManager.addLight( new SpotLight( color, position, direction, 80.0f, 0.85f) );  
  /**/
  
  CHECKGLERROR();
}
