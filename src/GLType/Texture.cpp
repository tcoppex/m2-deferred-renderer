/**
 * 
 *    \file Texture.cpp  
 * 
 */
 

#include <GL/glew.h>
#include <cassert>
#include <tools/Image.hpp>
#include <tools/Timer.hpp>

#include "Texture.hpp"


#define ENABLE_TEXTURE_MIPMAP   1
#define ENABLE_TEXTURE_ANISOTROPICFILTERING 1


/** TEXTURE ----------------------------------------- */

void Texture::generate()
{
  if (!m_id) {
    glGenTextures( 1, &m_id);
  }
}

void Texture::destroy()
{
  if (m_id)
  {
    glDeleteTextures( 1, &m_id);
    m_id = 0u;
  }
}

void Texture::bind(GLuint unit) const
{
  assert( 0u != m_id );  
  glActiveTexture( GL_TEXTURE0 + unit );
  glBindTexture( getTarget(), m_id);
}

void Texture::unbind(GLuint unit) const
{
  glActiveTexture( GL_TEXTURE0 + unit );
  glBindTexture( getTarget(), 0u);
}

void Texture::Unbind(GLenum target, GLuint unit)
{
  glActiveTexture( GL_TEXTURE0 + unit );
  glBindTexture( target, 0u);
}



/** TEXTURE 2D -------------------------------------- */

// to change
bool Texture2D::load( const std::string &name, 
                      bool bMipmap, bool bAnisotropy)
{
  generate();
  
  bMipmap &= ENABLE_TEXTURE_MIPMAP;
  bAnisotropy &= ENABLE_TEXTURE_ANISOTROPICFILTERING;
  
  bind();
  { 
    if (bMipmap) {
      SetFilterParameters( GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
    } else {
      SetFilterParameters( GL_LINEAR, GL_LINEAR);
    }
        
    if (bAnisotropy)
    {
      int maxAnisoLevel;        
      glGetIntegerv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisoLevel);
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisoLevel);
    }    
    
    Image_t image;    
    assert( true == image.load(name.c_str()) );
    //assert( GL_TEXTURE_2D == image.target ); //
    
    Texture::Image2D( image.internalFormat, image.width, image.height, 
                      image.format, image.type, image.data );
    
    if (bMipmap) {
      glGenerateMipmap( GL_TEXTURE_2D );
    }
  }
  unbind();
  
  
  return true;
}



/** TEXTURE CUBEMAP --------------------------------- */

bool TextureCubemap::load( const std::string &name, 
                           bool bMipmap, bool bAnisotropy)
{
/**
 * 
 */
 	
  generate();  
  
  bMipmap &= ENABLE_TEXTURE_MIPMAP;
  
  bind();
  {
    if (bMipmap) {
      glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else {
      glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
    
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    
    /// TODO : rewrite the loader-----------------------------------------------
    Image_t image[6];
    
    size_t wildcard_idx = name.find_last_of( '*', name.size());
    assert( wildcard_idx != name.npos );
    
    std::string begin_name = name.substr(0, wildcard_idx);
    std::string end_name = name.substr( wildcard_idx+1, name.size()-(wildcard_idx+1));
    static const std::string wildname[] = { "posx", "negx", "posy", "negy", "posz", "negz"};
    
    for (int i=0; i<6; ++i)
    {
      std::string texname = begin_name + wildname[i] + end_name;      
      
      assert( true == image[i].load(texname.c_str()) );      
      assert( GL_TEXTURE_2D == image[i].target ); //
      fprintf( stderr, "%s loaded\n", texname.c_str());
            
      glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 
                    image[i].internalFormat, 
                    image[i].width, image[i].height, 0, 
                    image[i].format, image[i].type, 
                    image[i].data);
    }
    /// TODO : rewrite the loader-----------------------------------------------
    
    if (bMipmap) {
      glGenerateMipmap( GL_TEXTURE_CUBE_MAP );
    }
  }
  unbind();
  
  return true;
}
