/**
 * 
 *    \file ProgramShader.cpp  
 * 
 */
 

#include <cstdio>
#include <cassert>

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glsw/glsw.h>

#include <tools/gltools.hpp>
#include <tools/Logger.hpp>

#include "ProgramShader.hpp"


ProgramShader* ProgramShader::m_pCurrent = 0;

void ProgramShader::generate()
{
  if (!m_id) {
    m_id = glCreateProgram();
  }
  
  #ifdef GL_ARB_separate_shader_objects
  glProgramParameteri( m_id, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_FALSE);
  glProgramParameteri( m_id, GL_PROGRAM_SEPARABLE, GL_FALSE);
  #endif
}

void ProgramShader::destroy()
{
  if (m_id)
  {
    glDeleteProgram( m_id );
    m_id = 0u;
  }
}


void ProgramShader::addShader(GLenum shaderType, const std::string &tag)
{
  assert( glswGetError() == 0 );
  
  
  const char* cTag = tag.c_str();
  const GLchar *source = glswGetShader( cTag );

  if (0 == source)
  {
    //Logger::getInstance().write( "Error : shader \"%s\" not found, check your directory.\n", cTag);
    fprintf( stderr, "Error : shader \"%s\" not found, check your directory.\n", cTag);
    fprintf( stderr, "Execution terminated.\n");
    exit( EXIT_FAILURE );
  }

  GLuint shader = glCreateShader( shaderType );
  glShaderSource( shader, 1, (const GLchar**)&source, 0);
  glCompileShader( shader );


  GLint status = 0;
  glGetShaderiv( shader, GL_COMPILE_STATUS, &status);

  if (status != GL_TRUE)
  {
    //Logger::getInstance().write( "shader \"%s\" compilation failed.\n", cTag);
    fprintf(stderr, "%s compilation failed.\n", cTag);
    gltools::printShaderLog( shader );
    exit( EXIT_FAILURE );
  }

  //Logger::getInstance().write( "%s compiled.\n", cTag);
  fprintf(stderr, "%s compiled.\n", cTag);
  
  glAttachShader( m_id, shader);       
  glDeleteShader( shader );     //flag for deletion
}



bool ProgramShader::link()
{
  glLinkProgram( m_id );

  // Test linking
  GLint status = 0;
  glGetProgramiv( m_id, GL_LINK_STATUS, &status);
  
  if (status != GL_TRUE)
  {
    fprintf(stderr, "program linking failed.\n");
    return false;
  }
  
  return true;
}




#define SETUNIFORM_HEADER() \
  GLint loc = glGetUniformLocation( m_id, name.c_str());\
  if (-1 == loc)\
  {\
    /*Logger::getInstance().debug( "ProgramShader : can't find uniform \"%s\".", name.c_str());*/\
    return false;\
  }


bool ProgramShader::setUniform( const std::string &name, GLint v) const
{
  SETUNIFORM_HEADER()
  glUniform1i( loc, v);
  
  return true;
}


bool ProgramShader::setUniform( const std::string &name, GLfloat v) const
{
  SETUNIFORM_HEADER()  
  glUniform1f( loc, v);
  
  return true;
}

bool ProgramShader::setUniform( const std::string &name, const glm::vec2 &v) const
{
  SETUNIFORM_HEADER()  
  glUniform2fv( loc, 1, glm::value_ptr(v));
  
  return true;
}

bool ProgramShader::setUniform( const std::string &name, const glm::vec3 &v) const
{
  SETUNIFORM_HEADER()
  glUniform3fv( loc, 1, glm::value_ptr(v));
  
  return true;
}

bool ProgramShader::setUniform( const std::string &name, const glm::vec4 &v) const
{
  SETUNIFORM_HEADER()
  glUniform4fv( loc, 1, glm::value_ptr(v));
  
  return true;
}

bool ProgramShader::setUniform( const std::string &name, const glm::mat3 &v) const
{
  SETUNIFORM_HEADER()
  glUniformMatrix3fv( loc, 1, GL_FALSE, glm::value_ptr(v));
  
  return true;
}

bool ProgramShader::setUniform( const std::string &name, const glm::mat4 &v) const
{
  SETUNIFORM_HEADER()
  glUniformMatrix4fv( loc, 1, GL_FALSE, glm::value_ptr(v));
  
  return true;
}




bool ProgramShader::setUniform( const std::string &name, GLint *v, int count) const
{
  SETUNIFORM_HEADER()
  glUniform1iv( loc, count, v);
  
  return true;
}

bool ProgramShader::setUniform( const std::string &name, GLfloat *v, int count) const
{
  SETUNIFORM_HEADER()
  glUniform1fv( loc, count, v);
  
  return true;
}

bool ProgramShader::setUniform( const std::string &name, glm::vec2 *v, int count) const
{
  SETUNIFORM_HEADER()
  glUniform2fv( loc, count, glm::value_ptr(*v));
  
  return true;
}

bool ProgramShader::setUniform( const std::string &name, glm::vec3 *v, int count) const
{
  SETUNIFORM_HEADER()
  glUniform3fv( loc, count, glm::value_ptr(*v));
  
  return true;
}

bool ProgramShader::setUniform( const std::string &name, glm::vec4 *v, int count) const
{
  SETUNIFORM_HEADER()
  glUniform4fv( loc, count, glm::value_ptr(*v));
  
  return true;
}

bool ProgramShader::setUniform( const std::string &name, glm::mat3 *v, int count) const
{
  SETUNIFORM_HEADER()
  glUniformMatrix3fv( loc, count, GL_FALSE, glm::value_ptr(*v));
  
  return true;
}

bool ProgramShader::setUniform( const std::string &name, glm::mat4 *v, int count) const
{
  SETUNIFORM_HEADER()
  glUniformMatrix4fv( loc, count, GL_FALSE, glm::value_ptr(*v));
  
  return true;
}

#undef SETUNIFORM_HEADER
