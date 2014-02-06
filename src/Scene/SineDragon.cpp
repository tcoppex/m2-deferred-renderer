
#include "SineDragon.hpp"
#include <GLType/ProgramShader.hpp>
#include <GLType/MatrixBuffer.hpp>
#include <GLType/VertexBuffer.hpp>
#include <Graphic/InstancedModel.hpp>
#include <Graphic/Mesh.hpp>
#include <tools/gltools.hpp>
#include <tools/Camera.hpp>
#include <tools/Timer.hpp>
#include <tools/AppConfig.hpp>
//#include <glm/gtx/fast_trigonometry.hpp>


SineDragon::~SineDragon()
{
  if (m_DragonBody) delete m_DragonBody;
  if (m_Program) delete m_Program;
  if (m_MotionBlurPS) delete m_MotionBlurPS;
}
    
void SineDragon::init()
{
  m_DragonBody = new InstancedModel();
  m_DragonBody->init( new CubeMesh(), MAX_INSTANCE, true);
  
  /// Init transforms
  _initMatrices();
  
  /// Load Program Shaders
  m_Program = new ProgramShader();
  m_Program->generate();
    m_Program->addShader( GL_VERTEX_SHADER, "SineDragon.Vertex");
    m_Program->addShader( GL_FRAGMENT_SHADER, "SineDragon.Fragment");
  m_Program->link();
  
  m_MotionBlurPS = new ProgramShader();
  m_MotionBlurPS->generate();
    m_MotionBlurPS->addShader( GL_VERTEX_SHADER, "ppMotionBlur.Instanced.Vertex");
    m_MotionBlurPS->addShader( GL_FRAGMENT_SHADER, "ppMotionBlur.FillBuffer.Fragment");
  m_MotionBlurPS->link();
  
  m_bInitialized = true;
}

void SineDragon::render(const Camera& camera)
{
  assert( m_bInitialized );
    
  if (AppConfig::GetInstance().getBool( "Dragon:Enable" ) == false)
    return;
  
  m_Program->bind();
  {
    m_Program->setUniform( "uViewProjMatrix", camera.getViewProjMatrix());
    m_Program->setUniform( "uViewMatrix", camera.getViewMatrix());
    m_Program->setUniform( "uNumInstance", int(m_numInstance));
    
    m_DragonBody->draw( m_numInstance );
  }
  m_Program->unbind();
  
  
  bool bMotionBlur = AppConfig::GetInstance().getBool( "ppMotionBlur:Enable" );
  
  // WHY U NO WORKS ?!
  if (bMotionBlur)
  {
    glDepthMask( GL_FALSE );//
    
    m_MotionBlurPS->bind();
    {
      m_Program->setUniform( "uProjectionMatrix", camera.getProjectionMatrix());
      m_Program->setUniform( "uViewMatrix", camera.getViewMatrix());
      
      m_DragonBody->draw( m_numInstance );
    }
    m_MotionBlurPS->unbind();
    
    glDepthMask( GL_TRUE );//
  }
  
  
  CHECKGLERROR();
}


void SineDragon::_initMatrices()
{
  MatrixBuffer4* pWorldMB = m_DragonBody->getWorldMatrixBuffer();
  MatrixBuffer3* pNormalMB = m_DragonBody->getNormalMatrixBuffer();
  MatrixBuffer4* pWorldMB_prev = m_DragonBody->getPrevWorldMatrixBuffer();
  
  std::vector<glm::mat4> &worlds = pWorldMB->getMatrices();  
  worlds.resize( m_numInstance, glm::mat4(1.0f) );
  
  std::vector<glm::mat3> &normals = pNormalMB->getMatrices();
  normals.resize( m_numInstance, glm::mat3(1.0f) );
  
  std::vector<glm::mat4> &worlds_prev = pWorldMB_prev->getMatrices();  
  worlds_prev.resize( m_numInstance, glm::mat4(1.0f) );
}

void SineDragon::update()
{
  if (AppConfig::GetInstance().getBool( "Dragon:Enable" ) == false)
    return;
    
  MatrixBuffer4* pWorldMB = m_DragonBody->getWorldMatrixBuffer();
  MatrixBuffer3* pNormalMB = m_DragonBody->getNormalMatrixBuffer();
  MatrixBuffer4* pWorldMB_prev = m_DragonBody->getPrevWorldMatrixBuffer();
  
  std::vector<glm::mat4> &worlds = pWorldMB->getMatrices();  
  std::vector<glm::mat3> &normals = pNormalMB->getMatrices();
  std::vector<glm::mat4> &worlds_prev = pWorldMB_prev->getMatrices();
  
  
  double time = Timer::GetInstance().getFrameTime();
  glm::vec3 K( 5.3f, 1.7f, 4.1f);
  
  /// Alternatively, try to update only the head & tails, & shift others
  for (size_t i=0; i<m_numInstance; ++i)
  {
    float t = (i + time) / 4000.0f;
    float spin = 5.0f*i + 0.1f*time;
    float dt = i/float(m_numInstance);
    
    const float radius = 70.0f;
    
    glm::vec3 pos;
    pos.x = 1.2*radius * glm::cos(K.x * t);
    pos.y = radius * glm::cos(K.y * t);
    pos.z = 1.5*radius * glm::sin(K.z * t);
  
    glm::mat4 &world = worlds[i];
    
    /// Previous position, for motion blur
    worlds_prev[i] = world;
    
    world = glm::mat4(1.0f);
    world = glm::translate( world, glm::vec3(0.0f, 30.0f, 0.0f));//    
    world = glm::translate( world, pos);
    world = glm::rotate( world, spin, glm::vec3(1.0f, 1.0f, 1.0f));
    world = glm::translate( world, glm::vec3(i/2000.));
    world = glm::translate( world, glm::vec3(50-15.0f*dt));
    
    /// It is costly to keep a normal matrix, but it helps when using scale matrices
    /// Here we could avoid using it.
    normals[i] = glm::mat3(world);
  }
  
  if (AppConfig::GetInstance().getBool( "rideDragon:Enable" ))
  {
    /// stick camera to the snake body 
    Camera &camera = Camera::GetInstance();
    glm::vec4 lambdaPos = glm::vec4(glm::vec3(0.0f), 1.0f);
    glm::vec4 pos = worlds[2*MAX_INSTANCE/4] * lambdaPos;
    glm::vec4 posNext = worlds[MAX_INSTANCE-1u] * lambdaPos;
    pos.y += 10.f;
    camera.setViewParams( glm::vec3(pos), glm::vec3(posNext));
  }
  
  pWorldMB->update();
  pNormalMB->update();
  pWorldMB_prev->update();
}
 
