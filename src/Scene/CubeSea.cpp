
#include "CubeSea.hpp"
#include <GLType/ProgramShader.hpp>
#include <GLType/MatrixBuffer.hpp>
#include <GLType/VertexBuffer.hpp>
#include <Graphic/InstancedModel.hpp>
#include <Graphic/Mesh.hpp>
#include <tools/gltools.hpp>
#include <tools/Camera.hpp>
#include <tools/Timer.hpp>
#include <glm/gtc/random.hpp>


#define DEFAULT 1

CubeSea::~CubeSea()
{
  if (m_Cubes) delete m_Cubes;
  if (m_Program) delete m_Program;
}
    
void CubeSea::init()
{
  m_Cubes = new InstancedModel();
  m_Cubes->init( new CubeMesh(), MAX_INSTANCE);
  
  _setTransformations();
  
  m_Program = new ProgramShader();
  m_Program->generate();
    // Use the SineDragon shader for simplicity
    m_Program->addShader( GL_VERTEX_SHADER, "SineDragon.Vertex");
    m_Program->addShader( GL_FRAGMENT_SHADER, "SineDragon.Debug.Fragment");
  m_Program->link();
  
  m_bInitialized = true;
}

void CubeSea::render(const Camera& camera)
{
  assert( m_bInitialized );
  
  m_Program->bind();
  {
    m_Program->setUniform( "uViewProjMatrix", camera.getViewProjMatrix());
    m_Program->setUniform( "uViewMatrix", camera.getViewMatrix());
    m_Program->setUniform( "uNumInstance", int(m_numInstance));
    
    m_Cubes->draw( m_numInstance );
  }
  m_Program->unbind();
  
  CHECKGLERROR();
}



void CubeSea::_setTransformations()
{ 
  
  MatrixBuffer4* pWorldMB = m_Cubes->getWorldMatrixBuffer();
  MatrixBuffer3* pNormalMB = m_Cubes->getNormalMatrixBuffer();
  
  std::vector<glm::mat4> &worlds = pWorldMB->getMatrices();  
  worlds.resize( m_numInstance, glm::mat4(1.0f) );
  
  std::vector<glm::mat3> &normals = pNormalMB->getMatrices();
  normals.resize( m_numInstance, glm::mat3(1.0f) );
  
  for (size_t i=0; i<m_numInstance; ++i)
  {
    glm::vec3 pos = glm::linearRand( glm::vec3(-4.0f, 0.0f, -4.0f), 
                                     glm::vec3(+4.0f, 5.0f, +4.0f));
    worlds[i] = glm::translate( glm::mat4(1.0f), pos);//
    normals[i] = glm::mat3(worlds[i]);
    
    worlds[i] = glm::scale( worlds[i], glm::vec3(0.02f));//
  }
  
  pWorldMB->update();
  pNormalMB->update();
}
