/**
 * 
 *    \file Texture.hpp  
 * 
 *    (Work in Progress)
 *    OpenGL Texture handler for 2D / Cubemap target
 * 
 *    look at nvidia-texture-tools for special processing.
 * 
 *    todo : # methods to change sampler parameters
 *           # handle mipmapping
 *           # handle anisotropic filtering
 *           # keep image data in the same structure ?
 *           # better loading process (for cubemaps)
 *           # replace some 'assert' by a more appropriate error handler 
 * 
 *           # handle rect texture independantly (from texture 2D) ?
 * 
 * 
 */
 

#pragma once

#ifndef GLTYPE_TEXTURE_HPP
#define GLTYPE_TEXTURE_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>


/** TEXTURE ----------------------------------------- */

class Texture
{
  protected:
    GLuint m_id;
  
  public:
    Texture() : m_id(0u) {}
    //explicit Texture(const std::string &name) { Texture(); load(name); }
    
    virtual ~Texture() {destroy();}
    
    
    /** Return the texture target */
    virtual GLenum getTarget() const = 0;
    
    /** Generate the texture id */
    void generate();
    
    /** Destroy the texture id */
    void destroy();
    
    /** Load the texture data from a file, call generate() if needed. */
    virtual bool load( const std::string &name, 
                       bool bMipmap, bool bAnisotropy) = 0;
    
    /** Return the texture id */
    GLuint getId() const {return m_id;}
  
    /** Bind the texture to the specified unit */
    void bind(GLuint unit=0u) const;
    
    /** Unbind the texture from the specified unit */
    void unbind(GLuint unit=0u) const;
    
    static
    void Unbind(GLenum target=GL_TEXTURE_2D, GLuint unit=0u);
    
    //void setAnisotropicLevel(int lvl);    
    
    //void enableMipMap();
    //void generateMipMap();

    static
    void Image2D( GLint internalFormat, GLsizei w, GLsizei h, GLenum format, 
                  GLenum type, const GLvoid *data)
    {
      glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, type, data);
    }

  private:
    //Texture(const Texture &) {}
    //Texture& operator =(const Texture &) const {}
};



/** TEXTURE 2D -------------------------------------- */

class Texture2D : public Texture
{
  protected:
    
  public:
    Texture2D() : Texture() {}    
    GLenum getTarget() const { return GL_TEXTURE_2D; }
    bool load( const std::string &name, 
               bool bMipmap=true, bool bAnisotropy=true);
    
    
    static
    void SetFilterParameters( GLint mag=GL_NEAREST, GLint min=GL_NEAREST)
    {
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min );
    }
    
    static
    void SetWrapParameters( GLint wrapS=GL_CLAMP_TO_EDGE, GLint wrapT=GL_CLAMP_TO_EDGE)
    {
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT );
    }
      
    //
    static
    void DefaultParameters()
    {
      SetFilterParameters();
      SetWrapParameters();
    }
};


/** TEXTURE CUBEMAP --------------------------------- */

class TextureCubemap : public Texture
{
  protected:
    //add irradiance envmap calculation ?
    
  public:
    TextureCubemap() : Texture() {}
    
    GLenum getTarget() const { return GL_TEXTURE_CUBE_MAP; }    
    bool load( const std::string &name,
               bool bMipmap=false, bool bAnisotropy=false);
};


#endif //GLTYPE_TEXTURE_HPP
