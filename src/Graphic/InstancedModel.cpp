/**
 * 
 *    \file InstancedModel.cpp 
 * 
 */


#include "InstancedModel.hpp"
#include "Mesh.hpp"
#include <GLType/MatrixBuffer.hpp>

   
    
InstancedModel::~InstancedModel()
{
  if (m_Mesh) delete m_Mesh;
  if (m_WorldMB) delete m_WorldMB;
  if (m_NormalMB) delete m_NormalMB;
  if (m_WorldMB_prev) delete m_WorldMB_prev;
}
    
void InstancedModel::init(Mesh *mesh, size_t maxInstance, bool bUseWorldPrev)
{
  assert( !m_bInitialized );
  assert( mesh != 0 );
  
  m_Mesh = mesh;
  m_Mesh->init();
  
  const VertexBuffer& vertexBuffer = m_Mesh->getVertexBuffer();
  vertexBuffer.bind();
  {
    GLuint location = NUM_VATTRIB;//
    
    m_WorldMB = new MatrixBuffer4();
    m_WorldMB->init( location, maxInstance, GL_DYNAMIC_DRAW);    
    location += m_WorldMB->getSize();
    
    m_NormalMB = new MatrixBuffer3();
    m_NormalMB->init( location, maxInstance, GL_DYNAMIC_DRAW);
    
    if (bUseWorldPrev)
    {
      m_WorldMB_prev = new MatrixBuffer4();
      m_WorldMB_prev->init( location, maxInstance, GL_DYNAMIC_DRAW);
      location += m_WorldMB_prev->getSize();
    }
  }
  vertexBuffer.unbind();
  
  
  m_bInitialized = true;
}

void InstancedModel::draw(size_t numInstance) const
{
  assert( m_bInitialized );
  assert( numInstance != 0 );
  
  m_Mesh->draw( numInstance );
}
