/**
 * 
 *    \file RenderTarget.hpp  
 * 
 *    \note use of 'complete' is not consistent with VertexBuffer's 'complete'
 *          method.
 * 
 */
 

#pragma once

#ifndef GLTYPE_RENDERTARGET_RENDERTARGET_HPP
#define GLTYPE_RENDERTARGET_RENDERTARGET_HPP

#include <GL/glew.h>


/**
 *  \class RenderTarget
 * 
 *  Abstract class used to render into a buffer.
 *  Currently, only for writing (target GL_FRAMEBUFFER).
 * 
 *  \see FrameBuffer, RenderTexture, RenderTextureDB
 */
class RenderTarget
{
  protected:
    GLuint m_fbo;
    
    // TODO : handle buffers resizing
    GLuint m_width;
    GLuint m_height;
    
    
  public:
    RenderTarget() : m_fbo(0u),
                     m_width(0u),
                     m_height(0u)
    {}
    
    virtual ~RenderTarget() {destroy();}
    
    inline void generate();
    
    inline virtual void destroy();
    
    //static?
    inline void complete();
    
    inline void bind() const;
    
    static
    inline void unbind();
    
    inline virtual void begin() const;//
    inline virtual void end() const;//
        
    inline virtual void resize(GLuint w, GLuint h);
    
    GLuint getId() const {return m_fbo;}
    GLuint getWidth() const {return m_width;}
    GLuint getHeight() const {return m_height;}
};




// INLINING XXX

#include <cassert>
#include <iostream>
#include <tools/gltools.hpp>


void RenderTarget::generate()
{
  if (!m_fbo) glGenFramebuffers( 1, &m_fbo);
}

void RenderTarget::destroy()
{
  if (m_fbo) 
  {
    glDeleteFramebuffers( 1, &m_fbo);
    m_fbo = 0u;
  }
}

void RenderTarget::complete()
{
  assert( m_fbo != 0u );
  //assert( (m_width != 0u) && (m_height != 0u) );  
  
  GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);  
  if (GL_FRAMEBUFFER_COMPLETE != result)
  {
    std::cerr << gltools::getGLErrorString(result) << std::endl;
    exit( EXIT_FAILURE );
  }
}

void RenderTarget::bind() const
{
  assert( m_fbo != 0u );
  glBindFramebuffer( GL_FRAMEBUFFER, m_fbo);
}

void RenderTarget::unbind()
{
  glBindFramebuffer( GL_FRAMEBUFFER, 0u);
}

void RenderTarget::begin() const 
{
  bind();
}

void RenderTarget::end() const 
{
  unbind();
}

void RenderTarget::resize(GLuint w, GLuint h) 
{
  m_width = w;
  m_height = h;
}

#endif //GLTYPE_RENDERTARGET_RENDERTARGET_HPP
