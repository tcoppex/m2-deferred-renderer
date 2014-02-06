/**
 * 
 *        ImageLoader.hpp
 * 
 *    Simple DevIL wrapper to load 2D & Rectangle image for OpenGL
 *    (stored as unsigned char). 
 *  
 *    yeaaah, that's awful
 * 
 *    (previously using FreeImage)
 * 
 *    TODO : # rewrite it completely to handle multiples internal format
 *           # Use nvdia-texture-tool
 */
 

#ifndef TOOLS_IMAGE_HPP
#define TOOLS_IMAGE_HPP

#include <GL/glew.h>
#include <IL/il.h>
#include <cassert>


struct Image_t
{
 private:
  static bool bINITIALISED;
    
 public:
	ILuint imgId;
  
  // ok, there will be information redundancy
  GLenum target;  
  GLint internalFormat;
  GLsizei width;
  GLsizei height;
  GLenum format;
  GLenum type;
  GLubyte *data;
  
  unsigned int bytesPerPixel;
  
  
  Image_t()
    : imgId(0u),
      target(GL_INVALID_ENUM),
      internalFormat(0),
      width(0), 
      height(0),
      format(GL_INVALID_ENUM),
      type(GL_INVALID_ENUM),
      data(0),
      bytesPerPixel(0u)
  {
    Init();
  }
  
  virtual ~Image_t();
  
  void clean();
  
  bool load(const char *filename);
  
  protected:
    bool _setDefaultAttributes();
  
  static
  void Init()
  {
    if (bINITIALISED) return;
    
    //assert( ilGetInteger(IL_VERSION_NUM) < IL_VERSION );
    ilInit();
    bINITIALISED = true;
  }
  
  private:
    Image_t(const Image_t&) {}
    const Image_t& operator=(const Image_t&) const {}
};

#endif //TOOLS_IMAGE_HPP
