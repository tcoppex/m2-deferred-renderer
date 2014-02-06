/**
 * 
 *    \file FrameBuffer.hpp  
 * 
 */
 

#pragma once

#ifndef GLTYPE_RENDERTARGET_FRAMEBUFFER_HPP
#define GLTYPE_RENDERTARGET_FRAMEBUFFER_HPP

#include <GL/glew.h>
#include <cstring>

#include "RenderTarget.hpp"
class Texture;


/**
 * \class FrameBuffer
 * 
 * Use to render scene datas into textures.
 * Texture must be supplies by the user.
 */
class FrameBuffer : public RenderTarget
{
  protected:
    // Actually glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, &num), but it's good for most hardwares.
    static const size_t ATTACHMENT_MAX = 8u;//OpenGL::Constant::MaxColorAttachments();
    
    /** Draw buffers' attachment point */
    GLenum m_attachments[ATTACHMENT_MAX];
    /** Draw buffers' texture */
    Texture* m_pTextures[ATTACHMENT_MAX];
    /** Number of buffer to draw */
    size_t m_numTextures;
    
    /** Texture target for a Depth/Stencil/DepthStencil buffer */
    Texture* m_pSpecialBuffer;
    /** Render buffer for a Depth/Stencil/DepthStencil buffer */
    GLuint m_specialRB;
    /** Format of the special buffer */
    GLenum m_specialFormat;
    
    
  public:
    FrameBuffer() : RenderTarget(),
                    m_numTextures(0u),
                    m_pSpecialBuffer(0),
                    m_specialRB(0u)
    {
      memset( m_attachments, 0, ATTACHMENT_MAX*sizeof(GLenum));
      memset( m_pTextures, 0, ATTACHMENT_MAX*sizeof(Texture*));
    }
    
    void destroy();
    
    void begin();
    void end();
    
    void resize(GLuint w, GLuint h);//
    
    void attachColorBuffer( GLenum attachment, Texture *pTexture);
    void attachSpecialBuffer( GLenum attachment, Texture *pTexture);
    
    void createRenderBuffer(GLenum attachment);
        
    Texture* getTexture(GLenum attachment) const;
  
  protected:
    bool _setSpecialFormat(GLenum attachment);
};

#endif //GLTYPE_RENDERTARGET_FRAMEBUFFER_HPP
