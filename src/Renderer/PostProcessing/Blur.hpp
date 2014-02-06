/**
 * 
 *
 */

#pragma once

#ifndef POSTPROCESSING_BLUR_HPP
#define POSTPROCESSING_BLUR_HPP

#include "PostEffect.hpp"
#include <tools/gltools.hpp>
#include <GLType/ProgramShader.hpp>
#include <GLType/Texture.hpp>
#include <GLType/RenderTarget/RenderTexture.hpp>


class Blur : public PostEffect
{
  protected:  
    int m_kernelSize;
    float m_falloff;
    float m_sharpness;
    
    // Alternatively, a RenderTargetDB could be used
    Texture2D m_hBlurTex;
    RenderTexture m_hBlurRT;
    
    const bool m_bUseDepth;//
    
    
  public:
  /*
    /// RED Blur wrt to neigbour's distance
    Blur( int kernelSize, float falloff, float sharpness)
      : PostEffect(),
        m_kernelSize(kernelSize),
        m_falloff(falloff),
        m_sharpness(sharpness),
        m_bUseDepth(true)
    {}
  */
  
    /// Generic RGB Blur
    explicit
    Blur( int kernelSize )
      : PostEffect(),
        m_kernelSize(kernelSize),        
        m_falloff(0.0f),
        m_sharpness(0.0f),
        m_bUseDepth(false)
    {}
    
    void init(const glm::vec2 &scale)
    {
      m_scale = scale;
      
      if (m_bUseDepth)
      {
        _defaultInit( "ppBlur.Depth.Fragment" );
      }
      else
      {
        _defaultInit( "ppBlur.Fragment" );
      }
      
      /// RenderTarget for the horizontal blur
      m_hBlurRT.create( &m_hBlurTex );
    }
    
    void resize( int w, int h)
    {
      w *= m_scale.x;
      h *= m_scale.y;
      
      /// verticale blur texture
      m_outputTex.generate();
      m_outputTex.bind();
      {
        // Automatic blur if render at lower res with LINEAR magnification
        Texture2D::SetFilterParameters( GL_NEAREST, GL_NEAREST);
        Texture2D::SetWrapParameters( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
        
        if (m_bUseDepth) {
          Texture::Image2D( GL_RED, w, h, GL_RED, GL_UNSIGNED_BYTE, 0);
        } else {
          Texture::Image2D( GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE, 0);
        }
      }
      m_outputTex.unbind();
      
      
      /// horizontale blur texture
      m_hBlurTex.generate();
      m_hBlurTex.bind();
      {
        Texture2D::SetFilterParameters( GL_LINEAR, GL_LINEAR);
        Texture2D::SetWrapParameters( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
        
        if (m_bUseDepth) {
          Texture::Image2D( GL_RED, w, h, GL_RED, GL_UNSIGNED_BYTE, 0);
        } else {
          Texture::Image2D( GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE, 0);
        }
      }
      m_hBlurTex.unbind();
    }
    
    
    float getFalloff() const {return m_falloff;}
    float getSharpness() const {return m_sharpness;}
    int getKernelSize() const {return m_kernelSize;}
    
    void setFalloff(float falloff) {m_falloff = falloff;}
    void setSharpness(float sharpness) {m_sharpness = sharpness;}
    void setKernelSize(int kernelSize) {m_kernelSize = kernelSize;}
    
    bool isEnable() const { return true; }
    
    protected:
      void _render(globalParams_t *pParams)
      {
        m_program.bind();
        {
          _setUniforms( pParams );
          
          /// First pass: horizontal blurring
          m_hBlurRT.begin();
          {
            m_program.setUniform( "uHorizontal", true);
            (*pParams).pInputTex->bind(0u);
            (*pParams).pDepthTex->bind(1u);
            gltools::DrawScreenQuad();
          }
          m_hBlurRT.end();          
          
          /// Second pass: vertical blurring                    
          m_renderTexture.begin();
          {
            m_program.setUniform( "uHorizontal", false);
            m_hBlurTex.bind( 0u );
            //(*pParams).pDepthTex->bind(1u); // already bound
            gltools::DrawScreenQuad();
          }
          m_renderTexture.end();
          
          Texture::Unbind( GL_TEXTURE_2D, 1u);
          Texture::Unbind( GL_TEXTURE_2D, 0u);
        }
        m_program.unbind();
      }
      
      void _setUniforms(globalParams_t *pParams)
      {
        m_program.setUniform( "uInputTex", 0);
        m_program.setUniform( "uDepthTex", 1);
                
        m_program.setUniform( "uFalloff", m_falloff);
        m_program.setUniform( "uSharpness", m_sharpness);
        m_program.setUniform( "uKernelSize", m_kernelSize);
        
        m_program.setUniform( "uLinearParams", (*pParams).linearParams);
      }
};

#endif //POSTPROCESSING_BLUR_HPP
