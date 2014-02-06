/**
 * 
 *      \file SSAO.hpp
 * 
 *    Interface for the Screen Space Ambient Occlusion post-processing
 *    effects.
 * 
 *    The SSAO effect is computed in 3 stages (the first one can be done 
 *    early in the pipeline):
 * 
 *    1) The occlusion term is computed. It needs a linearized depth buffer,
 *    a random texture (and potentially the normal buffer), it returns the
 *    occlusion term in the RED component.
 * 
 *    2) The occlusion term is blurred horizontally using the depth buffer to
 *       weight the result wrt the distance from surrounding fragments.
 * 
 *    3) The result is then blurred again, but vertically.
 *    
 * 
 */

#pragma once

#ifndef POSTPROCESSING_SSAO_HPP
#define POSTPROCESSING_SSAO_HPP

#include "PostEffect.hpp"
#include "Blur.hpp"//
#include <GLType/Texture.hpp>
#include <tools/gltools.hpp>
#include <tools/AppConfig.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>



class SSAO : public PostEffect
{
  protected:
    static const int MAX_SPHEREPOINTS = 32;
    glm::vec3 m_spherePoints[MAX_SPHEREPOINTS];
    
    /** Texture containing random vectors */
    static const int RANDTEX_RESOLUTION = 64;
    Texture2D m_randomTex;
    
    /** Number of sample to use per fragment */
    int m_numSample;
    
    float m_falloff;
    float m_radius;
    float m_factor;
    
    Blur *m_BlurEffect;
    
        
  public:
    SSAO()
      : PostEffect(),
        m_BlurEffect(0),
        m_numSample(8),
        m_falloff(0.0000005f),
        m_radius(0.0025f),
        m_factor(8.0f)
    {}
    
    ~SSAO()
    {
      m_randomTex.destroy();
      delete m_BlurEffect;
    }
    
    
    void init(const glm::vec2 &scale)
    {
      m_scale = scale;      
      _defaultInit( "ppSSAO.Fragment" );
      
      _initSpherePoints();
      _initRandomTex();
      
      const int kernelSize = 8;
      //const float falloff = 0.08f;//
      //const float sharpness = 10000.0f;//
      m_BlurEffect = new Blur( kernelSize/*, falloff, sharpness*/);
      m_BlurEffect->init( m_scale );
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
        Texture2D::SetFilterParameters( GL_NEAREST, GL_NEAREST);
        Texture2D::SetWrapParameters( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
        
        Texture::Image2D( GL_RED, w, h, GL_RED, GL_UNSIGNED_BYTE, 0);
      m_outputTex.unbind();
    }
    
    bool isEnable() const { return AppConfig::GetInstance().getBool( "ppSSAO:Enable" ); }
        
    protected:
      void _render(globalParams_t *pParams)
      {
        /// SSAO effect
        m_renderTexture.begin();
        {
          int texUnit = 0;
          (*pParams).pDepthTex->bind(texUnit++);
          (*pParams).pNormalTex->bind(texUnit++);
          m_randomTex.bind(texUnit++);
          
          m_program.bind();
            _setUniforms( pParams );
            gltools::DrawScreenQuad();
          m_program.unbind();
          
          Texture::Unbind( GL_TEXTURE_2D, --texUnit);
          Texture::Unbind( GL_TEXTURE_2D, --texUnit);
          Texture::Unbind( GL_TEXTURE_2D, --texUnit);
        }
        m_renderTexture.end();
        
        //
        /// Blur the output
        (*pParams).pInputTex = getOutput();
        m_BlurEffect->render( pParams );
        
        /// Set AO texture
        (*pParams).pAOTex = m_BlurEffect->getOutput();
        //
        
        
        // Beware, SSAO's getOutput() does not return the blurred texture !
      }
      
      
      void _setUniforms( globalParams_t *pParams )
      {
        int texUnit = 0;
        m_program.setUniform( "uDepthTex", texUnit++);
        m_program.setUniform( "uNormalTex", texUnit++);
        m_program.setUniform( "uRandTex", texUnit++);
        
        m_program.setUniform( "uSpherePoints", m_spherePoints, MAX_SPHEREPOINTS);
        
        /// Parameters
        m_program.setUniform( "uNumSample", m_numSample);
        m_program.setUniform( "uFalloff", m_falloff);
        m_program.setUniform( "uRadius", m_radius);
        m_program.setUniform( "uFactor", m_factor);
        
        const glm::vec2 texelSize = (1.0f / RANDTEX_RESOLUTION) * pParams->viewport;
        m_program.setUniform( "uRandTexelSize", texelSize);
        
        m_program.setUniform( "uLinearParams", (*pParams).linearParams);
      }


      /** Create random sample points around the unit sphere */
      void _initSpherePoints()
      {
        for (int i=0; i<MAX_SPHEREPOINTS; ++i) {
          m_spherePoints[i] = glm::sphericalRand( 1.0f );
        }
      }
      
      /** Create a normalized noise texture */
      void _initRandomTex()
      {
        const int resSqr = RANDTEX_RESOLUTION*RANDTEX_RESOLUTION;
        
        GLubyte *randTex = new GLubyte[3u*resSqr];  
        for (int i=0; i<resSqr; ++i)
        {
          glm::vec3 v = glm::linearRand( glm::vec3(0.0f), glm::vec3(1.0f));
          v = glm::normalize( v );
          
          randTex[3*i + 0] = GLubyte(255 * v.x);
          randTex[3*i + 1] = GLubyte(255 * v.y);
          randTex[3*i + 2] = GLubyte(255 * v.z);
        }
        
        m_randomTex.generate();
        m_randomTex.bind();
          Texture2D::SetFilterParameters( GL_NEAREST, GL_NEAREST);
          Texture2D::SetWrapParameters( GL_REPEAT, GL_REPEAT);
          
          Texture::Image2D( GL_RGB8, RANDTEX_RESOLUTION, RANDTEX_RESOLUTION, 
                            GL_RGB, GL_UNSIGNED_BYTE, randTex);
        m_randomTex.unbind();
        
        delete [] randTex;
      }
};

#endif //POSTPROCESSING_SSAO_HPP
