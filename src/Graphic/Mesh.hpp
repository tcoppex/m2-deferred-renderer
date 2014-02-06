/**
 * 
 *    \file Mesh.hpp
 * 
 *     \todo : Placer la geometrie GPU (VAO, VBO,..) dans une structure et
 *             liée Matrice + BufferObject dans un même ensemble.
 *             Donc simplifier le concept de 'Mesh' à l'objet seul, sans 
 *             transformation.
 * 
 */
 

#pragma once

#ifndef GRAPHIC_MESH_HPP
#define GRAPHIC_MESH_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLType/VertexBuffer.hpp>



/** MESH ---------------------------------------------- */

class Mesh
{
  protected:
    bool m_bInitialized;    
    
    VertexBuffer m_vertexBuffer;
    GLsizei m_count;
    
    
  public:
    Mesh()
      : m_bInitialized(false)
    {}
    
    virtual ~Mesh() {}
    
    virtual void init() {}
    
    virtual void draw() const {}
    
    /** for instanced geometry (we can also avoid overriding..)*/
    virtual void draw(size_t numInstance) const {}
    
    const VertexBuffer& getVertexBuffer() {return m_vertexBuffer;}//
};


/** PLANE MESH ----------------------------------------- */

class PlaneMesh : public Mesh
{
  protected:
    float m_size;
    float m_resolution;
    
  public:
    PlaneMesh(float size=100.0f, float res=32.0f)//
      : Mesh(),
        m_size(size),
        m_resolution(res)
    {}
    
    void init();
    void draw() const;
    void draw(size_t numInstance) const;
};


/** SPHERE MESH ---------------------------------------- */

class SphereMesh : public Mesh
{
  protected:
    int m_resolution;
    float m_radius;
    
  public:
    SphereMesh(int res=8, float radius=1.0f)
      : Mesh(),
        m_resolution(res),
        m_radius(radius)
    {}
    
    void init();
    void draw() const;
    void draw(size_t numInstance) const;
};


/** DOME MESH ---------------------------------------- */

class DomeMesh : public Mesh
{
  protected:
    int m_resolution;
    float m_radius;
    
  public:
    DomeMesh(int res=8, float radius=1.0f)
      : Mesh(),
        m_resolution(res),
        m_radius(radius)
    {}
    
    void init();
    void draw() const;
    void draw(size_t numInstance) const;
};


/** CONE MESH ------------------------------------------ */

class ConeMesh : public Mesh
{
  public:
    ConeMesh()
      : Mesh()
    {}
    
    void init();
    void draw() const;
    void draw(size_t numInstance) const;
};


/** CUBE MESH ------------------------------------------ */

class CubeMesh : public Mesh
{
  public:
    CubeMesh()
      : Mesh()
    {}
    
    void init();
    void draw() const;
    void draw(size_t numInstance) const;
};


#endif //GRAPHIC_MESH_HPP
