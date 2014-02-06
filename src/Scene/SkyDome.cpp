/**
 * 
 *    \file SkyDome.cpp
 * 
 */

#include "SkyDome.hpp"
#include <tools/gltools.hpp>
#include <tools/Camera.hpp>
#include <tools/Timer.hpp>
#include <GLType/ProgramShader.hpp>
#include <GLType/Texture.hpp>
#include <GLType/RenderTarget/RenderTexture.hpp>
#include <Graphic/Mesh.hpp>
#include <Renderer/PostProcess.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>



SkyDome::~SkyDome()
{
  if (!m_bInitialized) {
    return;
  }
  
  m_Program->destroy();
  delete m_Program;
    
  m_CloudTex->destroy();
  delete m_CloudTex;
    
  delete m_Dome;
}

void SkyDome::init()
{
  assert( !m_bInitialized );
  
  m_Program = new ProgramShader();
  m_Program->generate();
    m_Program->addShader( GL_VERTEX_SHADER, "SkyDome.Vertex" );
    m_Program->addShader( GL_FRAGMENT_SHADER, "SkyDome.Fragment" );
  m_Program->link();
  
  m_Dome = new DomeMesh( 24u, 1.0f);//
  m_Dome->init();
  
  
  //scaleY = 0.65f;
  m_scaleMatrix = glm::scale( glm::mat4(1.0f), glm::vec3(150.0f));
  
  _initTextures();  
  CHECKGLERROR();
  
  m_bInitialized = true;
}


void SkyDome::render(const Camera& camera)
{
  assert( m_bInitialized );
  
  glDisable( GL_DEPTH_TEST );//
  glDepthMask( GL_FALSE );//
  
  m_Program->bind();
  {
    _updateUniforms( camera );
    
    m_CloudTex->bind();
    m_Dome->draw(); 
    m_CloudTex->unbind();
  }
  m_Program->unbind(); 
  
  glDepthMask( GL_TRUE );//
  glEnable( GL_DEPTH_TEST );//
  
  CHECKGLERROR();
}



void SkyDome::_updateUniforms(const Camera &camera)
{
  glm::mat4 mvpMatrix = camera.getViewProjMatrix();    
  
  // center the dome to the camera (TODO: send the offset as uniform)
  glm::mat4 translation = glm::translate( glm::mat4(1.0f), camera.getPosition());
  
  mvpMatrix = mvpMatrix * translation * m_scaleMatrix;  
  m_Program->setUniform( "uModelViewProjMatrix", mvpMatrix);
  
  float timer = Timer::GetInstance().getFrameTime() / 1000.0f;
  timer *= m_cloudSpeed;
  m_Program->setUniform( "uTimer", timer);
  
  m_Program->setUniform( "uCloudTex", 0);
  m_Program->setUniform( "uSkyColor", m_color);
}


void SkyDome::_initTextures()
{
  // ~~ TODO: delegate to an other objects
  
  
  const int res = 512;
  
  m_CloudTex = new Texture2D();
    
  /// Initialize the cloud texture parameters
  m_CloudTex->generate();
  m_CloudTex->bind();
  {
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, res, res, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glGenerateMipmap( GL_TEXTURE_2D ); 
  }
  m_CloudTex->unbind();
  
  /// The cloud is generate in a preprocess pass using a Noise function
  ProgramShader cloudProgram;//
  cloudProgram.generate();
    cloudProgram.addShader( GL_VERTEX_SHADER, "PostProcess.Vertex" );
    cloudProgram.addShader( GL_FRAGMENT_SHADER, "SkyDome.CloudTex.Fragment" );
    cloudProgram.addShader( GL_FRAGMENT_SHADER, "Noise.Include" );
  cloudProgram.link();
  
  /// A RenderTexture is used to generate the cloud texture into a FBO
  RenderTexture cloudRT;//
  cloudRT.create( m_CloudTex );
  
  /// PreProcess pass
  cloudRT.begin();
  cloudProgram.bind();
  {
    cloudProgram.setUniform( "uEnableTiling", true);
    cloudProgram.setUniform( "uTileRes", glm::vec3(res));
    int seed = glm::linearRand(0.0f, 456789.0f);//
    cloudProgram.setUniform( "uPermutationSeed", seed);
    
    gltools::DrawScreenQuad();
  }
  cloudProgram.unbind();
  cloudRT.end();
  
  
  // Automatic
  //cloudRT.destroy();
  //cloudProgram.destroy();
  
  CHECKGLERROR();
}


