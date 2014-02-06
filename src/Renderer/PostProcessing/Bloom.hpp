/**
 *
 *    improvment : merge with 'SSAO'
 */

#ifndef POSTPROCESSING_BLOOM_HPP
#define POSTPROCESSING_BLOOM_HPP

#include "PostEffect.hpp"
#include "Blur.hpp"//
#include <tools/gltools.hpp>
#include <GLType/Texture.hpp>


class Bloom : public PostEffect
{
  protected:
    Blur *m_BlurEffect;
    
  public:
    Bloom() 
      : PostEffect(),
        m_BlurEffect(0)
    {}
    
    ~Bloom()
    {
      delete m_BlurEffect;
    }
    
    void init(const glm::vec2 &scale)
    {
      m_scale = scale;
      _defaultInit( "ppBloom.Fragment" );
      
      //
      const int kernelSize = 20;
      //const float falloff = 0.08f;
      //const float sharpness = 10000.0f;
      m_BlurEffect = new Blur( kernelSize /*, falloff, sharpness*/ );
      m_BlurEffect->init( m_scale );
      //
    }
    
    void resize(int w, int h)
    {
      if (m_BlurEffect) {
        m_BlurEffect->resize( w, h);
      }
      
      w *= m_scale.x;
      h *= m_scale.y;
      
      m_outputTex.generate();
      m_outputTex.bind();
        Texture2D::SetFilterParameters( GL_LINEAR, GL_LINEAR);
        Texture2D::SetWrapParameters( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
        Texture::Image2D( GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE, 0);
      m_outputTex.unbind();
    }
    
    bool isEnable() const { return AppConfig::GetInstance().getBool( "ppBloom:Enable" ); }
        
    protected:
      void _render(globalParams_t *pParams)
      {
        m_renderTexture.begin();
        {
          (*pParams).pSceneTex->bind(0u);
          (*pParams).pDepthTex->bind(1u);
          
          m_program.bind();
            _setUniforms( pParams );
            gltools::DrawScreenQuad();
          m_program.unbind();
          
          Texture::Unbind( GL_TEXTURE_2D, 1u);
          Texture::Unbind( GL_TEXTURE_2D, 0u);
        }
        m_renderTexture.end();
        
        //
        /// Blur the output
        (*pParams).pInputTex = getOutput();
        m_BlurEffect->render( pParams );
        
        /// Set Bloom texture
        (*pParams).pBloomTex = m_BlurEffect->getOutput();
        //
      }
      
      void _setUniforms( globalParams_t *pParams )
      {
        m_program.setUniform( "uSceneTex", 0);
        m_program.setUniform( "uDepthTex", 1);//
        
        /// Parameters to linearize the depth buffer
        m_program.setUniform( "uLinearParams", (*pParams).linearParams);
      }
};

#endif //POSTPROCESSING_BLOOM_HPP

