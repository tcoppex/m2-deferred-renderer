

#pragma once

#ifndef POSTPROCESSING_FINAL_HPP
#define POSTPROCESSING_FINAL_HPP

#include "PostEffect.hpp"
#include <tools/gltools.hpp>
#include <tools/AppConfig.hpp>
#include <GLType/ProgramShader.hpp>
#include <GLType/Texture.hpp>


class Final : public PostEffect
{
  public:
    Final() 
      : PostEffect() 
    {}
    
    
    void init(const glm::vec2 &scale)
    {
      m_scale = scale;
      
      /// Create & link the main program
      m_program.generate();
        m_program.addShader( GL_VERTEX_SHADER, "PostProcess.Vertex");
        m_program.addShader( GL_FRAGMENT_SHADER, "PostProcess.Fragment");
      m_program.link();
    }
    
    void resize(int w, int h)
    {
    }
        
    bool isEnable() const {return true;}
    
    protected:
      void _render(globalParams_t *pParams)
      {
        /// Note: Final effect does not render into a texture !
        
        int texUnit = 0;
        (*pParams).pSceneTex->bind(texUnit++);
        (*pParams).pInputTex->bind(texUnit++);
        if ((*pParams).pAOTex) (*pParams).pAOTex->bind(texUnit++);
        if ((*pParams).pBloomTex) (*pParams).pBloomTex->bind(texUnit++);//
        if ((*pParams).pDoFTex) (*pParams).pDoFTex->bind(texUnit++);//
        
        //XXX
        (*pParams).pMotionTex->bind(texUnit++);//
        //XXX
                
        m_program.bind();
          _setUniforms( pParams );
          gltools::DrawScreenQuad();
        m_program.unbind();
        
        while (texUnit>0) {
          Texture::Unbind( GL_TEXTURE_2D, --texUnit);
        }
      }
      
      
      void _setUniforms( globalParams_t *pParams )
      {
        // XXX redo
        bool bSSAO = (*pParams).pAOTex != 0;
        bool bBloom = (*pParams).pBloomTex != 0;
        bool bDoF = (*pParams).pDoFTex != 0;
                
        int texUnit = 0;
        m_program.setUniform( "uSceneTex", texUnit++);
        m_program.setUniform( "uInputTex", texUnit++);
        if (bSSAO) m_program.setUniform( "uAOTex", texUnit++); //
        if (bBloom) m_program.setUniform( "uBloomTex", texUnit++); //
        if (bDoF) m_program.setUniform( "uDoFTex", texUnit++); //
        
        // XXX
        m_program.setUniform( "uMotionTex", texUnit++); //
        // XXX
        
        /// State which effects are enabled
        m_program.setUniform( "uEnableAO", bSSAO);
        m_program.setUniform( "uEnableBloom", bBloom);
        m_program.setUniform( "uEnableDoF", bDoF);
        
        AppConfig& config = AppConfig::GetInstance();
        m_program.setUniform( "uEnableVignetting", config.getBool( "ppVignetting:Enable" ));
        m_program.setUniform( "uEnableGammaCorrection", config.getBool( "ppGammaCorrection:Enable" ));//
        //m_program.setUniform( "uEnableSepiaFilter", config.getBool( "PostProcess:SepiaFilter" ));//
        //m_program.setUniform( "uEnableGreyscaleFilter", config.getBool( "PostProcess:GreyscaleFilter" ));//
        
        /// Other useful params
        m_program.setUniform( "uTime", pParams->time);
        m_program.setUniform( "uRand", pParams->rand);
        m_program.setUniform( "uTexelSize", 1.0f / (pParams->viewport));
        
        float gamma = config.getFloat( "Gamma" );
        m_program.setUniform( "uInvGamma", 1.0f / gamma);
      }
};

#endif //POSTPROCESSING_FINAL_HPP
