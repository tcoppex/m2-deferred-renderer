/**
 * 
 *    \file RenderTexture.cpp  
 * 
 */
 

#include "RenderTexture.hpp"
#include <GLType/Texture.hpp>
#include <cassert>


/* RenderTexture -------------------------------------------------------------*/

void RenderTexture::create(Texture2D* pOutputTex)
{
  generate();
  bind();
    setOutputTexture(pOutputTex);
    complete();
  unbind();
}


void RenderTexture::setOutputTexture(Texture2D *pTexture) 
{
  assert( (pTexture != 0) );
  
  m_pTexture = pTexture;  
  glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pTexture->getId(), 0);
} 
  


/* RenderTextureDB -----------------------------------------------------------*/

void RenderTextureDB::create(Texture2D* pInputTex, Texture2D* pOutputTex)
{
  generate();
  bind();
    setInputTexture(pInputTex);
    setOutputTexture(pOutputTex);
    complete();
  unbind();
}

void RenderTextureDB::setInputTexture(Texture2D *pTexture)
{
  assert( (pTexture != 0) && (pTexture != m_pTexture) );
  
  m_pTextureBis = pTexture;
  glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, pTexture->getId(), 0);
}
