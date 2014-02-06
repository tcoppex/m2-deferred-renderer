/**
 * 
 *    \file InstanceMatrixBuffer.hpp  
 * 
 *    
 *  Holds a buffer of matrices for instanced geometry.
 * 
 */
 

#pragma once

#ifndef GLTYPE_MATRIXBUFFER_HPP
#define GLTYPE_MATRIXBUFFER_HPP

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>


/**
 * \class AbstractMatrixBuffer
 * 
 */
//template<typename BaseType, int numberOfRow>
class AbstractMatrixBuffer
{
  protected:
    GLuint m_vbo;            
    size_t m_maxInstance;
    GLuint m_location;

  public:
    AbstractMatrixBuffer() 
      : m_vbo(0u),
        m_maxInstance(0u),
        m_location(0u)
    {}
    
    virtual ~AbstractMatrixBuffer() {_destroy();}
    
    
    /** *///a VAO need to be bounded
    void init(GLuint location, size_t maxInstance, GLenum usage);
    
    /** */
    virtual void update() = 0;
    
    /** */
    virtual void cleanData() = 0;
    
    /** */
    GLuint getVBO() const {return m_vbo;}
    
    /** */
    size_t getMaxInstance() {return m_maxInstance;}
    
    /** Return the buffer's vertex attrib location */
    GLuint getLocation() {return m_maxInstance;}
    
    /** Return the number of vertex attrib location slots taken */
    virtual GLuint getSize() const = 0;
    
    
  protected:  
    void _generate() {
      if (!m_vbo) glGenBuffers( 1, &m_vbo);
    }
    
    void _destroy();
};


class MatrixBuffer4 : public AbstractMatrixBuffer
{
  protected:
    std::vector<glm::mat4> m_matrices;
    
  public:
    MatrixBuffer4()
      : AbstractMatrixBuffer()
    {}
    
    
    void update();
    
    void cleanData() {m_matrices.clear();}
        
    GLuint getSize() const {return 4u;}
    
    std::vector<glm::mat4>& getMatrices() {return m_matrices;}
};


class MatrixBuffer3 : public AbstractMatrixBuffer
{
  protected:
    std::vector<glm::mat3> m_matrices;
    
  public:
    MatrixBuffer3() 
      : AbstractMatrixBuffer() 
    {}
    
    
    void update();
    
    void cleanData() {m_matrices.clear();}
            
    GLuint getSize() const {return 3u;}
    
    std::vector<glm::mat3>& getMatrices() {return m_matrices;}
};


#endif //GLTYPE_MATRIXBUFFER_HPP
