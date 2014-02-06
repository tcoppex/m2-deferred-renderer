/**
 * 
 *      \file WaterPlane.cpp
 * 
 *  not the best..
 * 
 */
 

#include "WaterPlane.hpp"
#include <Graphic/Model.hpp>
#include <Graphic/Mesh.hpp>
#include <GLType/ProgramShader.hpp>
#include <GLType/Texture.hpp>
#include <GLType/RenderTarget/FrameBuffer.hpp>
#include <tools/Timer.hpp>
#include <glm/gtc/random.hpp>
#include <cstdio>

 

void WaterPlane::init()
{
  m_Plane = new Model();
  m_Plane->init( new PlaneMesh(500, 100) );
  
  m_Program = new ProgramShader();
  m_Program->generate();
    m_Program->addShader( GL_VERTEX_SHADER, "Water.Vertex");
    m_Program->addShader( GL_FRAGMENT_SHADER, "Water.Fragment");
  m_Program->link();
  
  m_color = glm::vec4( 1.0f );
    
  _initWaves();
  _initReflectionRT();
}

void WaterPlane::_initWaves()
{
  const float medianWavelength = 25.0f;
  const float medianAmplitude = 0.07f;
  const float two_pi = 710.0f / 113.0f;
 
  float wavelength = 0.0f;
   
  for (int i=0; i<MAX_WAVE; ++i)
  {
    waveData_t &wave = m_waves[i];
    
    float factor = 1.5f * glm::linearRand(0.0f, 1.0f) + 1.0f;
    wavelength = factor * (0.5f * medianWavelength);
    
    wave.frequency = two_pi / wavelength;
    //wave.frequency = sqrtf( wave.frequency );
    
    wave.phase = wavelength * wave.frequency;
    wave.amplitude = medianAmplitude * (wavelength / medianWavelength);
    
    wave.direction = glm::circularRand(1.0f);//glm::diskRand(1.0f);
  }
  
  #if 1
  m_waves[0].frequency = 0.1f;
  m_waves[0].amplitude = 0.5f;
  m_waves[0].phase = 0.8f;
  m_waves[0].direction = glm::vec2(-1.0f, 0.0f);
    
  m_waves[1].frequency = 0.2f;
  m_waves[1].amplitude = 0.6f;
  m_waves[1].phase = 1.3f;
  m_waves[1].direction = glm::vec2(-0.7f, 0.7f);
      
  m_waves[2].frequency = 0.45f;
  m_waves[2].amplitude = 0.7f;
  m_waves[2].phase = 0.75f;
  m_waves[2].direction = glm::vec2(0.2f, -0.35f);
  #endif
  
  m_numWave = 5;
}

void WaterPlane::_initReflectionRT()
{
  const int res = 512;
  
  m_ReflectionTex = new Texture2D();
  m_ReflectionTex->generate();
  m_ReflectionTex->bind();
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);//GL_CLAMP_TO_EDGE
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, res, res, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  m_ReflectionTex->unbind();
  
  m_ReflectionFB = new FrameBuffer(); 
  m_ReflectionFB->generate();
  
  m_ReflectionFB->bind();
  {
    m_ReflectionFB->resize( res, res);
    m_ReflectionFB->attachColorBuffer( GL_COLOR_ATTACHMENT0, m_ReflectionTex);
    m_ReflectionFB->createRenderBuffer( GL_DEPTH_ATTACHMENT );    
    m_ReflectionFB->complete();
  }
  m_ReflectionFB->unbind();
}

void WaterPlane::_destroy()
{
  if (m_Plane) delete m_Plane;
  if (m_Program) delete m_Program;
  if (m_ReflectionTex) delete m_ReflectionTex;
  if (m_ReflectionFB) delete m_ReflectionFB;
}


void WaterPlane::beginReflection(Camera& camera)
{
  assert( m_bRenderingReflection == false );  
  
  glm::mat4 reflectMatrix = glm::scale( glm::mat4(1.0f), glm::vec3( 1.0f, -1.0f, 1.0f));
  
  
  // BAD XXX
    
  /// save view params
  m_oldView = camera.getViewMatrix();
  
  /// set REFLECTED view
  camera.setViewMatrix( camera.getViewMatrix() * reflectMatrix );  //
  
  /// save projection params
  camera.getProjectionParams( &m_projFov, &m_projZNear, &m_projZFar);
  
  /// set Oblique Frustum clipPlane
  // TODO
  
  /// save viewport  
  glGetIntegerv( GL_VIEWPORT, m_viewport);
  
  /// set new Viewport
  glViewport( 0, 0, m_ReflectionFB->getWidth(), m_ReflectionFB->getHeight());
  
  m_ReflectionFB->begin();
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
  m_bRenderingReflection = true;
}


// TODO
void WaterPlane::endReflection(Camera& camera)
{
  m_bRenderingReflection = false;
  m_ReflectionFB->end();
  
  /// reset viewport
  glViewport( m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
  
  /// reset view
  camera.setViewMatrix( m_oldView );//
  
  /// reset projection
  camera.setProjectionParams( m_projFov, m_viewport[2]/float(m_viewport[3]), m_projZNear, m_projZFar);
}






void WaterPlane::render(const Camera& camera)
{
  /// Don't render the water plane if in reflection mode.
  if (m_bRenderingReflection) {
    return;
  }
  
  static 
  const glm::mat4 BIAS = glm::translate( glm::mat4(1.0f), glm::vec3(0.5f)) * 
                         glm::scale( glm::mat4(1.0f), glm::vec3(0.5f));  

  glm::mat4 model = m_Plane->getWorldMatrix();
  
  /// Centers the animated plane to the camera
  glm::vec3 pos = camera.getPosition();
  glm::mat4 translate = glm::translate( glm::mat4(1.0f), glm::vec3(pos.x, 0.0f, pos.z));
  //model *= translate;
  
  const glm::mat4 &mvp = camera.getViewProjMatrix() * model;
  const glm::mat3 &normalView = m_Plane->getNormalMatrix() * glm::mat3(camera.getViewMatrix());
  
  const glm::mat4 &projTexMatrix = BIAS * mvp;
  
  
  m_ReflectionTex->bind();
  
  m_Program->bind();
  {
    /// Generic matrices for transformation
    m_Program->setUniform( "uModelViewProjMatrix", mvp);
    m_Program->setUniform( "uNormalViewMatrix", normalView);
    
    /// TexCoord projection matrix
    m_Program->setUniform( "uProjTexMatrix", projTexMatrix);
    
    m_Program->setUniform( "uEye", pos);
    
    /// Time for the simulation step
    float time = Timer::GetInstance().getFrameTime() / 1000.0f;    
    m_Program->setUniform( "uTime", time);
    
    /// Waves' parameters
    _setWavesUniforms();
    
    /// Fragment's uniforms
    m_Program->setUniform( "uReflectTex", 0);
    m_Program->setUniform( "uWaterColor", m_color);
    
    
    m_Plane->draw();
  }
  m_Program->unbind();
  
  m_ReflectionTex->unbind();
}


void WaterPlane::_setWavesUniforms()
{
  static char buffer[64];
  char *pBase = 0;
  
  for (int i=0; i<MAX_WAVE; ++i)
  {
    int idx = sprintf( buffer, "uWaves[%d].", i);
    pBase = &(buffer[idx]);
    
    sprintf( pBase, "amplitude");
    m_Program->setUniform( buffer, m_waves[i].amplitude);
    
    sprintf( pBase, "frequency");
    m_Program->setUniform( buffer, m_waves[i].frequency);
    
    sprintf( pBase, "phase");
    m_Program->setUniform( buffer, m_waves[i].phase);
    
    sprintf( pBase, "direction");
    m_Program->setUniform( buffer, m_waves[i].direction);
  }
  
  m_Program->setUniform( "uNumWave", m_numWave);
}
