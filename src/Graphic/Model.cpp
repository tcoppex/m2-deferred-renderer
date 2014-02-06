/**
 * 
 *  \file Model.cpp
 * 
 * 
 */
 
#include "Model.hpp"
#include "Mesh.hpp"
#include <cassert>


Model::Model()
  : m_bInitialized(false),
    m_Mesh(0)
{
}

Model::~Model()
{
  if (m_Mesh) delete m_Mesh;
  m_Mesh = 0;
}

void Model::init(Mesh *mesh)
{
  assert( !m_bInitialized );
  assert( mesh != 0 );
  
  m_Mesh = mesh;
  m_Mesh->init();
  
  m_bInitialized = true;
}

void Model::draw() const
{
  assert( m_bInitialized );  
  
  m_Mesh->draw();
}
