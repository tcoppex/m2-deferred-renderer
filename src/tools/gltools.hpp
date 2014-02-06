/**
 *
 *        \file gltools.hpp
 * 
 *        Some utility function for OpenGL
 *
 */


#pragma once

#ifndef TOOLS_GLTOOLS_HPP
#define TOOLS_GLTOOLS_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>


#ifndef NDEBUG
  #define CHECKGLERROR()    gltools::checkGLError( __FILE__, __LINE__, "", true);
  #define CHECKVALUE(x)     gltools::checkValue( __FILE__, __LINE__, #x, x);
#else
  #define CHECKGLERROR()
  #define CHECKVALUE(x)
#endif

#ifndef SAFE_DELETE
  #define SAFE_DELETE(x)  if (x) {delete (x); x = 0;}
#endif


namespace gltools
{
  const char* getGLErrorString(GLenum err);
  void    checkGLError(const char *file, const int line, const char *errMsg="", bool bExitOnFail=false);
  GLint   getShaderError( GLuint shader, GLenum pname );
  GLvoid  printShaderLog(GLuint shader);
  GLint   getProgramError( GLuint program, GLenum pname );
  GLvoid  printProgramLog( GLuint program );  
    
  GLuint  createShader( GLenum shaderType, const char *shaderTag);
  
  
  // Handle some glUniform function
  void    setUniform( GLuint program, const char *name, int value);
  void    setUniform( GLuint program, const char *name, float value);
  void    setUniform( GLuint program, const char *name, const glm::vec3 &value);
  void    setUniform( GLuint program, const char *name, const glm::vec4 &value);
  void    setUniform( GLuint program, const char *name, const glm::mat3 &value);
  void    setUniform( GLuint program, const char *name, const glm::mat4 &value);
  
  void    checkValue(const char *file, const int line, const char *name, bool value);
  
  /** This does not draw a screen quad. Instead, it sends 3 randoms vertice to
   *  the GPU. A vertex can then handle this vertice to draw a triangle with
   *  texcoords set to map texture as a quad will do.*/
  void DrawScreenQuad();
}

#endif //TOOLS_GLTOOLS_HPP
