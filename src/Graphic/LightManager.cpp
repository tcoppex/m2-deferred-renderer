/**
 *             
 *                \file LightManager.cpp
 * 
 * 
 * 
 */
 

#include "LightManager.hpp"
#include "Light.hpp"
#include "Mesh.hpp"
#include <GLType/ProgramShader.hpp>
#include <tools/gltools.hpp>
#include <tools/Camera.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <cstring>


LightManager::~LightManager()
{
  clearAll();
  
  glDeleteQueries( 1, &m_queryLightSamples);
}

void LightManager::init()
{
  m_meshes[LIGHT_POINT] = new SphereMesh( 32, 1.0f);
  m_meshes[LIGHT_POINT]->init();
  
  m_meshes[LIGHT_SPOT] = new ConeMesh();
  m_meshes[LIGHT_SPOT]->init();
  
  m_meshes[LIGHT_DIRECTIONAL] = new CubeMesh();//
  m_meshes[LIGHT_DIRECTIONAL]->init();
  
  memset( m_pForwardLight, 0, MAX_FORWARD_LIGHT * sizeof(Light*));
  
  glGenQueries( 1, &m_queryLightSamples);
}

void LightManager::render(LightRenderMode mode, const Camera& camera)
{
  switch (mode)
  {
    case LRM_GEOMETRY:
      _renderGeometry(camera);
    break;
    
    case LRM_DEFERREDPASS:
      _deferredLightPass(camera);
    break;
    
    default:
      assert(0);
    break;
  }
}

void LightManager::update()
{
  /*
  if (m_pSun)
  {
    static float spin = 0.0f;
    
    glm::mat4 rot = glm::rotate( glm::mat4(1.0f), spin, glm::vec3(1.0f, 0.0f, 0.0f));
    m_pSun->position = glm::vec3(0.0f, 1.0f, 0.0f) * glm::mat3(rot);
    m_pSun->position.y = glm::abs(m_pSun->position.y);
    spin += 10.0f;
  }
  */
}

void LightManager::setForwardLightUniforms(const Camera &camera)
{
  static char buffer[64u];
  int light_id=0;
  
  for (size_t i=0u; i<m_numForwardLights; ++i)
  {
    Light *pLight = m_pForwardLight[i];
    
    if (!pLight || !(pLight->bEnable)) continue;
    
    int idx = sprintf( buffer, "uLights[%d].", light_id);
    
    pLight->setForwardUniforms( camera, buffer, &(buffer[idx]) );    
    ++light_id;
  }
  
  ProgramShader::GetCurrent().setUniform( "uNumLight", light_id);
}

int LightManager::addLight( Light *light )
{
  assert( 0 != light );
  
  m_SceneLights.push_back( light );
  
  // ..
  if (light->bForwardLight && (m_numForwardLights < MAX_FORWARD_LIGHT))
  {
    m_pForwardLight[m_numForwardLights] = light;
    ++m_numForwardLights;    
  }
  
  if ((light->getType() == LIGHT_DIRECTIONAL) && (m_pSun == 0))
  {
    m_pSun = static_cast<DirectionalLight*>(light);
  }
  
  return m_SceneLights.size()-1;//
}

void LightManager::clearAll()
{
  m_numForwardLights = 0u;
  
  std::list<Light*>::iterator it;  
  for (it = m_SceneLights.begin(); it != m_SceneLights.end(); ++it) {
    delete *it;
  }
}


void LightManager::_deferredLightPass(const Camera& camera)
{
  assert( false == m_SceneLights.empty());
  

  glEnable( GL_STENCIL_TEST );  
  glEnable( GL_CULL_FACE );
    
  glBlendEquation( GL_FUNC_ADD );
  glBlendFunc( GL_ONE, GL_ONE );
  
    
  std::list<Light*>::iterator it;
  for (it=m_SceneLights.begin(); it!=m_SceneLights.end(); ++it)
  {
    Light* light = *it;
    
    if (false == light->bEnable) {
      continue;
    }
    
    // Send light uniforms to GPU
    light->setDeferredUniforms( camera );  //
    
    
    // =============== Directional lights ===============
    if (light->getType() == LIGHT_DIRECTIONAL)
    {
      glDisable( GL_CULL_FACE );
      glDisable( GL_STENCIL_TEST );
      
        _drawLightshape( light );
      
      glEnable( GL_CULL_FACE );
      glEnable( GL_STENCIL_TEST );
      
      continue;
      
      /*
      glFrontFace( GL_CCW );
      glDisable( GL_STENCIL_TEST );
      */
    }
    else
    {
      /*
      glFrontFace( GL_CW );
      glEnable( GL_STENCIL_TEST );
      */
    }
    
        
    // =============== non-directional lights ===============
    
    glClear( GL_STENCIL_BUFFER_BIT );
        
    // PASS 1    
    glCullFace( GL_FRONT );  
    glDepthFunc( GL_GEQUAL );

    glStencilFunc( GL_ALWAYS, 1, 0xff);
    glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE);
      
    glColorMask( 0, 0, 0, 0);
      _drawLightshape( light );
    glColorMask( 1, 1, 1, 1);  
        

    // PASS 2      
    if (light->intersectPoint(camera.getPosition()))
    {
      glDepthFunc( GL_ALWAYS );
    } 
    else 
    {
      glCullFace( GL_BACK );
      glDepthFunc( GL_LEQUAL );
    }

    glStencilFunc( GL_EQUAL, 1, 0xff);
    glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP);

    glEnable( GL_BLEND );    
    
    glBeginQuery( GL_SAMPLES_PASSED, m_queryLightSamples);
    _drawLightshape( light );
    glEndQuery( GL_SAMPLES_PASSED );
        
    glDisable( GL_BLEND );
    
    /*
    GLuint numSamplesPassed = 0u;
    glGetQueryObjectuiv( m_queryLightSamples, GL_QUERY_RESULT, &numSamplesPassed);
    
    float percent = 100.0f * numSamplesPassed * (1.0f / (WINDOW_WIDTH * WINDOW_HEIGHT));//
    if ((percent > 0.1f) && (light->isCastingShadows()) {
      //m_CastingShadowsLights.push_back( light );
    }
    */
  }
    
  glDisable( GL_CULL_FACE );
  glDisable( GL_STENCIL_TEST );
  
  glDepthFunc( GL_LEQUAL );
  
  
  CHECKGLERROR();
}


void LightManager::_renderGeometry(const Camera& camera)
{
  if (m_SceneLights.empty()) {
    return;
  }  
    
  glDepthMask( GL_FALSE );//
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);//
  
  std::list<Light*>::iterator light;  
  for (light=m_SceneLights.begin(); light!=m_SceneLights.end(); ++light)
  {      
    (*light)->setDeferredUniforms( camera );//
    _drawLightshape( *light );
  }
  
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);//
  glDepthMask( GL_TRUE );//  
}


void LightManager::_drawLightshape(Light* pLight)
{ 
  #if 0
  
  m_meshes[pLight->getType()]->draw();
  
  #else
  
  LightType lightType = pLight->getType();
  
  if (lightType == LIGHT_DIRECTIONAL)
  {
    gltools::DrawScreenQuad();
  }
  else
  {
    m_meshes[lightType]->draw();
  }
  
  #endif
}
