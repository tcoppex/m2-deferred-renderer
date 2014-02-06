/**
 * 
 *    \file DoF.hpp
 * 
 */

#ifndef POSTPROCESSING_DOF_HPP
#define POSTPROCESSING_DOF_HPP

#include "PostEffect.hpp"
#include <tools/gltools.hpp>
#include <GLType/Texture.hpp>


class DoF : public PostEffect
{
  protected:
    /*static const*/
    glm::vec2 m_kernels[12];

    float m_dofAmount;
    float m_focalDepth;
    float m_noBlurRange;      // no blur in focalDepth +/- noBlurRange
    float m_maxBlurRange;


  public:
    DoF()
      : PostEffect()
    {}
    
    void init(const glm::vec2 &scale)
    {
      m_scale = scale;      
      _defaultInit( "ppDoF.Fragment" );
      
      m_kernels[ 0] = glm::vec2( 1.0f, 0.0f);
      m_kernels[ 1] = glm::vec2( 0.5f, 0.8660f);
      m_kernels[ 2] = glm::vec2(-0.5f, 0.8660f);
      m_kernels[ 3] = glm::vec2(-1.0f, 0.0f);
      m_kernels[ 4] = glm::vec2(-0.5f,-0.8660f);
      m_kernels[ 5] = glm::vec2( 0.5f,-0.8660f);      
      m_kernels[ 6] = glm::vec2( 1.5f, 0.8660f);
      m_kernels[ 7] = glm::vec2( 0.0f, 1.7320f);
      m_kernels[ 8] = glm::vec2(-1.5f, 0.8660f);
      m_kernels[ 9] = glm::vec2(-1.5f,-0.8660f);
      m_kernels[10] = glm::vec2( 0.0f,-1.7320f);
      m_kernels[11] = glm::vec2( 1.5f,-0.8660f);
    }
    
    void resize(int w, int h)
    {
      w *= m_scale.x;
      h *= m_scale.y;
      
      m_outputTex.generate();
      m_outputTex.bind();
        Texture2D::SetFilterParameters( GL_LINEAR, GL_LINEAR);
        Texture2D::SetWrapParameters( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
        
        Texture::Image2D( GL_RGB8, w, h, GL_RGB, GL_UNSIGNED_BYTE, 0);
      m_outputTex.unbind();
    }
    
    bool isEnable() const { return AppConfig::GetInstance().getBool( "ppDoF:Enable" ); }
    
    protected:
      void _render(globalParams_t *pParams)
      {
        /// SSAO effect
        m_renderTexture.begin();
        {
          int texUnit = 0;
          (*pParams).pSceneTex->bind(texUnit++);
          (*pParams).pDepthTex->bind(texUnit++);
          
          m_program.bind();
          {
            _setUniforms( pParams );
            gltools::DrawScreenQuad();
          }
          m_program.unbind();
          
          while (texUnit>0) {
            Texture::Unbind( GL_TEXTURE_2D, --texUnit);
          }
        }
        m_renderTexture.end();
        
        (*pParams).pDoFTex = getOutput();
      }
      
      void _setUniforms(globalParams_t *pParams) 
      {
        /// Textures' unit
        assert( m_program.setUniform( "uSceneTex", 0) > -1 );
        assert( m_program.setUniform( "uDepthTex", 1) > -1 );
        
        /// Parameters to linearize the depth buffer
        m_program.setUniform( "uLinearParams", (*pParams).linearParams);
        
        m_program.setUniform( "uKernels", m_kernels, 12);
        m_program.setUniform( "uNumSamples", 12);
      }
      
};

#endif //POSTPROCESSING_DOF_HPP
