/**
 * 
 *    \file RenderTexture.hpp  
 * 
 * 
 *    \todo handle cubemap textures
 * 
 *    \note For this object, I'm not sure if the texture must be allocate 
 *          internally or externally. [for the moment: externally]
 */
 

#pragma once

#ifndef GLTYPE_RENDERTARGET_RENDERTEXTURE_HPP
#define GLTYPE_RENDERTARGET_RENDERTEXTURE_HPP

#include "RenderTarget.hpp"
#include <GL/glew.h>
class Texture2D;


/**
 * \class RenderTexture
 * 
 *  Used to create texture (ie, w/ hardware depth information).
 *  m_pTexture : texture wich will be write into.
 */
class RenderTexture : public RenderTarget
{
  protected:
    Texture2D *m_pTexture;    
  
  public:
    RenderTexture() : RenderTarget(),
                      m_pTexture(0)
    {}
    
    /// Create the RT from an external texture.
    void create(Texture2D* pOutputTex);
        
    virtual void begin()
    {
      bind();
      glDrawBuffer( GL_COLOR_ATTACHMENT0 );
      //glDisable(GL_DEPTH_TEST);
      //glDepthMask( GL_FALSE );      
      //glClear( GL_COLOR_BUFFER_BIT );
    }
    
    //void end();
    
    void setOutputTexture(Texture2D *pTexture);
    
    virtual Texture2D* getOutputTexture() const { return m_pTexture; }
    
};


/**
 * \class RenderTextureDB
 * 
 *  Used to create texture (ie, w/ hardware depth information) with
 *  double buffering.
 * 
 *  By default, m_pTexture is the Output texture,
 *              m_pTextureBis is the Input texture
 */
class RenderTextureDB : public RenderTexture
{
  protected:
    Texture2D *m_pTextureBis;
    bool m_bBisOut;
    bool m_bAutoSwap;
  
  public:
    RenderTextureDB(bool bAutoSwap=false) 
      : RenderTexture(),
        m_pTextureBis(0),
        m_bBisOut(false),
        m_bAutoSwap(bAutoSwap)
    {}
    
    void create(Texture2D* pInputTex, Texture2D* pOutputTex);
    
    void begin()
    {
      bind();
      glDrawBuffer( (m_bBisOut)?GL_COLOR_ATTACHMENT1:GL_COLOR_ATTACHMENT0 );
    }
    
    void end()
    {
      unbind();
      if (m_bAutoSwap) swap();
    }
        
    void setInputTexture(Texture2D *pTexture);
    
    Texture2D* getInputTexture() const { return (!m_bBisOut)?m_pTextureBis:m_pTexture; }
    Texture2D* getOutputTexture() const { return (m_bBisOut)?m_pTextureBis:m_pTexture; }
    
    void swap() { m_bBisOut = !m_bBisOut; }
    void reset() { m_bBisOut = false; }
};


#endif //GLTYPE_RENDERTARGET_RENDERTEXTURE_HPP
