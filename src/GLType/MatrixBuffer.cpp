/**
 * 
 *    \file MatrixBuffer.cpp
 * 
 */
 
#include "MatrixBuffer.hpp"


/** MATRIX BUFFER ------------------------------------------- */

void AbstractMatrixBuffer::init(GLuint location, size_t maxInstance, GLenum usage)
{
  assert( maxInstance != 0u );
  
  
  _generate();
  
  m_maxInstance = maxInstance;
  m_location = location;
  
  glBindBuffer( GL_ARRAY_BUFFER, m_vbo);
  {
    const GLsizei vectorSize = getSize() * sizeof(GLfloat);
    const GLsizei stride = getSize() * vectorSize;
    
    const GLsizeiptr bufferSize = stride * m_maxInstance;
    glBufferData( GL_ARRAY_BUFFER, bufferSize, 0, usage);
        
    GLintptr offset = 0;    
    for (size_t i=0; i<getSize(); ++i)
    {
      glEnableVertexAttribArray(m_location + i);//
      glVertexAttribPointer( m_location+i, getSize(), GL_FLOAT, GL_FALSE, stride, 
                             (const GLvoid*)(offset));
      
      glVertexAttribDivisor( m_location+i, 1);
      
      offset += vectorSize;
    }
  }
  glBindBuffer( GL_ARRAY_BUFFER, 0u);
}


void AbstractMatrixBuffer::_destroy()
{
  if (m_vbo)
  {
    glDeleteBuffers( 1, &m_vbo);
    m_vbo = 0u;
  }
}

/** MATRIX BUFFER 4 ----------------------------------------- */

void MatrixBuffer4::update()
{
  assert( !m_matrices.empty() );
  
  
  GLsizeiptr bufferSize = glm::min( m_matrices.size(), m_maxInstance);
  bufferSize *= getSize() * getSize() * sizeof(GLfloat);
  
  glBindBuffer( GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData( GL_ARRAY_BUFFER, 0, bufferSize, &m_matrices[0]);
  glBindBuffer( GL_ARRAY_BUFFER, 0u);
}


/** MATRIX BUFFER 3 ----------------------------------------- */

void MatrixBuffer3::update()
{
  assert( !m_matrices.empty() );
  
  
  GLsizeiptr bufferSize = glm::min( m_matrices.size(), m_maxInstance);  
  bufferSize *= getSize() * getSize() * sizeof(GLfloat);
  
  glBindBuffer( GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData( GL_ARRAY_BUFFER, 0, bufferSize, &m_matrices[0]);
  glBindBuffer( GL_ARRAY_BUFFER, 0u);
}
