/**
 * 
 *    \file MotionBlur.hpp
 * 
 */

#pragma once

#ifndef POSTPROCESSING_MOTIONBLUR_HPP
#define POSTPROCESSING_MOTIONBLUR_HPP

#include "PostEffect.hpp"
#include <tools/gltools.hpp>
#include <GLType/Texture.hpp>


class MotionBlur : public PostEffect
{
  protected:
    float m_blurScale;
    int m_numSamples;
    
  public:
    MotionBlur( float blurScale=1.0f, int nSamples=16 )
      : PostEffect(),
        m_blurScale(blurScale),
        m_numSamples(nSamples)
    {}
    
    void init(const glm::vec2 &scale)
    {
      m_scale = scale;      
      _defaultInit( "ppMotionBlur.Fragment" );
    }
    
    void resize( int w, int h)
    {
      w *= m_scale.x;
      h *= m_scale.y;
      
      m_outputTex.generate();
      m_outputTex.bind();
      {
        Texture2D::SetFilterParameters( GL_NEAREST, GL_NEAREST);
        Texture2D::SetWrapParameters( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
        
        Texture::Image2D( GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE, 0);
      }
      m_outputTex.unbind();
    }
    
    bool isEnable() const { return false; /*AppConfig::GetInstance().getBool( "ppMotionBlur:Enable");*/ }
    
    protected:
      void _render(globalParams_t *pParams)
      {         
        m_renderTexture.begin();
        {
          (*pParams).pSceneTex->bind(0u);
          (*pParams).pMotionTex->bind(1u);
          
          m_program.bind();
          {
            _setUniforms( pParams );
            gltools::DrawScreenQuad();
          }
          m_program.unbind();
          
          Texture::Unbind( GL_TEXTURE_2D, 1u);
          Texture::Unbind( GL_TEXTURE_2D, 0u);
        }
        m_renderTexture.end();
        
        // Replace scene by the MoBlurred texture ?
        //(*pParams).pSceneTex = getOutput();
        
        //for debugging
        //(*pParams).pMoBlurTex = getOutput();//
      }
      
      void _setUniforms(globalParams_t *pParams)
      {
        m_program.setUniform( "uSceneTex", 0);
        m_program.setUniform( "uMotionTex", 1);
        
        m_program.setUniform( "uBlurScale", m_blurScale);
        m_program.setUniform( "uBlurNumSamples", m_numSamples);
        
        m_program.setUniform( "uViewport", (*pParams).viewport);
      }
};

#endif //POSTPROCESSING_MOTIONBLUR_HPP
