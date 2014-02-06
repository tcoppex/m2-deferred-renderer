/**
 * 
 *    \file PostEffect.hpp
 * 
 * 
 * 
 *      \note Instead of loading shaders, they can be set statically in each
 *            specialization.
 * 
 *            Previous implementation was more generics but now, as an
 *            effect can include multiple pass (eg, for blurring), nothing
 *            is implemented.
 * 
 *            Alternatively, we could set the different pass of an effect in 
 *            different Effects (with adjustement to link effects with each others).
 * 
 *      \todo compile PostProcess.Vertex a single time.
 */


#ifndef POSTPROCESSING_POSTEFFECT_HPP
#define POSTPROCESSING_POSTEFFECT_HPP

#include "globalParams.h"

#include <App.hpp> //

#include <GLType/RenderTarget/RenderTexture.hpp>
#include <GLType/ProgramShader.hpp>
#include <GLType/Texture.hpp>
//class ProgramShader;
//class Texture2D;
//class RenderTexture;


class PostEffect
{
  protected:    
    // All this Ressources handler could be included in a RessourceManager.. (TODO)
    ProgramShader m_program;
    Texture2D m_outputTex;
    RenderTexture m_renderTexture;    
    
    /// Effect's output resolution compared to screen resolution
    glm::vec2 m_scale;
    
    
  public:
    PostEffect() :
      m_scale(glm::vec2(1.0f))
    {}
    
    
    virtual ~PostEffect()
    {
      m_program.destroy();
      m_outputTex.destroy();
      m_renderTexture.destroy();
    }
    
    virtual void init(const glm::vec2 &scale = glm::vec2(1.0f)) = 0;
        
    /** Create RT textures & change their resolutions if needed */
    virtual void resize(int w, int h) = 0;
        
    /** Template function to render the effect into the texture */
    virtual void render(globalParams_t *pParams)
    {
      /// Change viewport
      int w = App::ScreenWidth;
      int h = App::ScreenHeight;      
      glm::vec2 vp( m_scale.x * w, m_scale.y * h);
      glViewport( 0, 0, vp.x, vp.y);//
      (*pParams).viewport = vp;
      
      _render( pParams );
      
      /// The generated texture become the new input
      //(*pParams).pInputTex = getOutput();//
      
      /// Reset viewport
      glViewport( 0, 0, w, h);//
      (*pParams).viewport = glm::vec2( w, h);
    }
    
    
    Texture2D* getOutput() { return &m_outputTex; }//
    
    virtual bool isEnable() const = 0;//
    
    const glm::vec2& getScale() const {return m_scale;}
    void setScale(const glm::vec2 &scale) {m_scale = scale;}
    
        
    protected: 
      void _defaultInit(const char *fragmentTag)
      {
        /// Create & link the main program
        m_program.generate();
          m_program.addShader( GL_VERTEX_SHADER, "PostProcess.Vertex");
          m_program.addShader( GL_FRAGMENT_SHADER, fragmentTag);
        m_program.link();
        
        /// Create & resize textures buffer if needed.
        int w = App::ScreenWidth;
        int h = App::ScreenHeight;
        resize( w, h);
        
        /// Create & complete the RenderTarget
        m_renderTexture.create( &m_outputTex );
      }
            
      /** Set the textures & uniforms and render the effect */
      virtual void _render( globalParams_t *pParams ) = 0;
      
      /** Set the program's uniform parameters */
      virtual void _setUniforms( globalParams_t *pParams ) = 0;
};

#endif //POSTPROCESSING_POSTEFFECT_HPP
