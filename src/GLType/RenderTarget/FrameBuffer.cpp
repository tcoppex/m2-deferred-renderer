/**
 * 
 *    \file FrameBuffer.cpp
 * 
 */
 

#include "FrameBuffer.hpp"
#include <GLType/Texture.hpp>
#include <cassert>




void FrameBuffer::destroy()
{
  if (m_fbo) 
  {
    glDeleteFramebuffers( 1, &m_fbo);
    m_fbo = 0u;
  }
  
  if (m_specialRB) 
  {
    glDeleteRenderbuffers( 1, &m_specialRB);
    m_specialRB = 0u;
  }
}

void FrameBuffer::begin()
{
  bind();
  glDrawBuffers( m_numTextures, m_attachments);
}

void FrameBuffer::end()
{
  //glDrawBuffer( GL_NONE );
  unbind();
}

void FrameBuffer::resize(GLuint w, GLuint h)
{
  assert( (w != 0u) && (h != 0u) );
  
  m_width = w;
  m_height = h;
  
  // As texture are set externally, only the renderbuffer must be updated.
  if (m_specialRB)
  {
    glBindRenderbuffer( GL_RENDERBUFFER, m_specialRB);
      glRenderbufferStorage( GL_RENDERBUFFER, m_specialFormat, m_width, m_height);
    glBindRenderbuffer( GL_RENDERBUFFER, 0u);
  }
  
  // not sure if I had to resize texture here or let the user do it..
  // (They are pointers and they can be used for others things)
}

void FrameBuffer::attachColorBuffer( GLenum attachment, Texture *pTexture)
{
  assert( pTexture != 0 );
  
  size_t id = attachment - GL_COLOR_ATTACHMENT0;
  assert( (id >= 0) && (id < ATTACHMENT_MAX) );
  
  if (0 == m_attachments[id]) {
    m_numTextures += 1u;
  }
  
  m_attachments[id] = attachment;
  m_pTextures[id] = pTexture;
  
  /* Versions 3.x+ of OpenGL seem to accept indifferently TEXTURE_2D / TEXTURE_RECTANGLE
   * as texture target, but it can be a problem for older version. */
  glFramebufferTexture2D( GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, pTexture->getId(), 0);
}

void FrameBuffer::attachSpecialBuffer( GLenum attachment, Texture *pTexture)
{
  assert( (m_specialRB == 0u) && (m_pSpecialBuffer == 0));
  
  assert( _setSpecialFormat(attachment) );
  
  m_pSpecialBuffer = pTexture;  
  glFramebufferTexture2D( GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, pTexture->getId(), 0);
}


void FrameBuffer::createRenderBuffer(GLenum attachment)
{
  assert( (m_specialRB == 0u) && (m_pSpecialBuffer == 0));

  // Generate the render buffer if needed.
  if (!m_specialRB) glGenRenderbuffers( 1, &m_specialRB);
  
  // Set the internal format depending on the attachment.
  assert( _setSpecialFormat(attachment) );
   
  glBindRenderbuffer( GL_RENDERBUFFER, m_specialRB);
    glRenderbufferStorage( GL_RENDERBUFFER, m_specialFormat, m_width, m_height);
  glBindRenderbuffer( GL_RENDERBUFFER, 0u);
  
  // Attach the render buffer as a logical buffer to the currently bound FBO.
  glFramebufferRenderbuffer( GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, m_specialRB);
}

Texture* FrameBuffer::getTexture(GLenum attachment) const
{
  size_t id = attachment - GL_COLOR_ATTACHMENT0;
  assert( (id >= 0) && (id < ATTACHMENT_MAX) );
  
  return m_pTextures[id];
}

bool FrameBuffer::_setSpecialFormat(GLenum attachment)
{
  switch (attachment)
  {
    case GL_DEPTH_ATTACHMENT:
      m_specialFormat = GL_DEPTH_COMPONENT;
    break;
    
    case GL_DEPTH_STENCIL_ATTACHMENT:
      m_specialFormat = GL_DEPTH24_STENCIL8;
    break;
    
    default:
      return false;
  } 
  
  return true;
}
