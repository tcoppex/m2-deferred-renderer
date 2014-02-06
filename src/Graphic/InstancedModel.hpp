/**
 * 
 *    \file InstancedModel.hpp 
 * 
 */
 

#pragma once

#ifndef GRAPHIC_INSTANCEDMODEL_HPP
#define GRAPHIC_INSTANCEDMODEL_HPP

#include <glm/glm.hpp>
class Mesh;
class MatrixBuffer4;
class MatrixBuffer3;


class InstancedModel
{
  protected:  
    bool m_bInitialized;
    Mesh *m_Mesh;
  
    MatrixBuffer4 *m_WorldMB;    
    MatrixBuffer3 *m_NormalMB;
    
    /// Matrices used for motion blur
    MatrixBuffer4 *m_WorldMB_prev;
    
    glm::vec3 m_color;
    //bool m_bWorldAsNormal;
    
    
  public:
    InstancedModel()
      : m_bInitialized(false),
        m_Mesh(0),
        m_WorldMB(0), 
        m_NormalMB(0),
        m_WorldMB_prev(0)
    {}
        
    virtual ~InstancedModel();
    
    void init(Mesh *mesh, size_t maxInstance, bool bUseWorldPrev=false);
    
    void draw(size_t numInstance) const;
    
    // SETTERS
    void setColor(const glm::vec3 &color) {m_color = color;}
    
    // GETTERS
    MatrixBuffer4* getWorldMatrixBuffer() { return m_WorldMB; }
    MatrixBuffer3* getNormalMatrixBuffer() { return m_NormalMB; }
    MatrixBuffer4* getPrevWorldMatrixBuffer() { return m_WorldMB_prev; }
};

#endif //GRAPHIC_INSTANCEDMODEL_HPP
