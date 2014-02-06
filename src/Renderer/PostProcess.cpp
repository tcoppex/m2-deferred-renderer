/**
 *             
 *        \file PostProcess.cpp
 * 
 */


#include "PostProcess.hpp"

#include "PostProcessing/Final.hpp"
#include "PostProcessing/SSAO.hpp"
#include "PostProcessing/Blur.hpp"
#include "PostProcessing/Bloom.hpp"
#include "PostProcessing/MotionBlur.hpp"
//#include "PostProcessing/DoF.hpp"

#include <GLType/RenderTarget/RenderTexture.hpp>
#include <GLType/ProgramShader.hpp>
#include <GLType/Texture.hpp>
#include <tools/gltools.hpp>
#include <tools/Camera.hpp>
#include <tools/Timer.hpp>
#include <App.hpp>//



PostProcess::PostProcess()
  : m_bInitialized(false),
    m_FinalEffect(0)
{}
  
PostProcess::~PostProcess()
{
  if (!m_bInitialized) {
    return;
  }
  
  if (m_FinalEffect) delete m_FinalEffect;
  
  postEffectList_t::iterator it;
  for (it=m_ppEffects.begin(); it!=m_ppEffects.end(); ++it) {
    delete *it;
  }
}

void PostProcess::init()
{
  glm::vec2 scale = glm::vec2( 1.0f );
  
  m_FinalEffect = new Final();
  m_FinalEffect->init();
  
  
  PostEffect *pEffect = 0;  
  
  pEffect = new SSAO();
  pEffect->init( scale );
  m_ppEffects.push_back( pEffect );
  
  pEffect = new Bloom();
  pEffect->init( 0.25f*scale );
  m_ppEffects.push_back( pEffect );
  
  
  // [WiP]
  pEffect = new MotionBlur();
  pEffect->init( scale );
  m_ppEffects.push_back( pEffect );
        
  /*
  // UNFINISHED
  pEffect = new DoF();
  pEffect->init( scale );
  m_ppEffects.push_back( pEffect );
  /**/
  
  /**//*
  pEffect = new Blur( 16  );
  pEffect->init( 0.25f*scale );
  m_ppEffects.push_back( pEffect );
  /**/
  
  m_bInitialized = true;
} 

void PostProcess::run()
{
  assert( m_bInitialized );
    
  glDisable( GL_DEPTH_TEST );
  glDepthMask( GL_FALSE );
  
  
  //-------------------------------------------------------
  
  /// Update global params
  m_globalParams.reset();
  m_globalParams.time = Timer::GetInstance().getFrameTime();
  m_globalParams.rand = rand() / float(RAND_MAX);
  m_globalParams.linearParams = Camera::GetInstance().getLinearizationParams();
  
  m_globalParams.pInputTex = m_globalParams.pSceneTex;
  
  
  /// Render each effects
  if ((false == m_ppEffects.empty()))
  {  
    postEffectList_t::iterator it;
    for (it=m_ppEffects.begin(); it!=m_ppEffects.end(); ++it)
    {
      PostEffect *pEffect = *it;
      
      if (pEffect->isEnable()) {
        pEffect->render( &m_globalParams );
      }
    }
  }
  
  /// Render the final effect, compositing each effect in a final pass.
  m_FinalEffect->render( &m_globalParams );  
  
  //-------------------------------------------------------
    
  
  glDepthMask( GL_TRUE );
  glEnable( GL_DEPTH_TEST );
  
  CHECKGLERROR();
}

