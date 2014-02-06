/**
 * 
 *    \file Model.hpp 
 * 
 *    \todo implements physic handling (see bullet)
 * 
 */
 

#pragma once

#ifndef GRAPHIC_MODEL_HPP
#define GRAPHIC_MODEL_HPP

#include <glm/glm.hpp>
class Mesh;


class Model
{
  protected:  
    bool m_bInitialized;
    Mesh *m_Mesh;
    //btCollisionObject *m_CollisionObject;
  
    glm::mat4 m_worldMatrix;
    glm::mat3 m_normalMatrix;
    
    glm::vec3 m_color;    
    
  public:
    Model();
    virtual ~Model();
    
    void init(Mesh *mesh);    
    void draw() const;
    
    // SETTERS
    void setWorldMatrix(const glm::mat4 &world) {m_worldMatrix = world;}
    void setNormalMatrix(const glm::mat3 &normal) {m_normalMatrix = normal;}
    void setColor(const glm::vec3 &color) {m_color = color;}
    
    // GETTERS
    const glm::mat4& getWorldMatrix() const { return m_worldMatrix; }
    const glm::mat3& getNormalMatrix() const { return m_normalMatrix; }
    const glm::vec3& getColor() const {return m_color;}      
};


/**
 * A simple structure to handle models using a motion blur effects.
 * 
 * note: no normal matrix is set, instead the 3x3 model matrix is sent
 *       which is good in practice (no scale applied).
 */
struct moBlurModel_t
{
  public:  
    moBlurModel_t(Model *ptrModel) : pModel(ptrModel) 
    {
      prevWorldMatrix = pModel->getWorldMatrix();
    }
    
    Model *pModel;
    glm::mat4 prevWorldMatrix;
    
    void updateWorldMatrix(const glm::mat4& world)
    {
      prevWorldMatrix = pModel->getWorldMatrix();
      pModel->setWorldMatrix( world );
    }
};

#endif //GRAPHIC_MODEL_HPP
