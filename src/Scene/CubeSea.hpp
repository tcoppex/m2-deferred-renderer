/**
 * 
 *  \file CubeSea.hpp
 * 
 *  instanced set of cubes for test...
 */
 

#ifndef SCENE_CUBESEA_HPP
#define SCENE_CUBESEA_HPP

#include <cstdlib>
class ProgramShader;
class InstancedModel;
class Camera;


class CubeSea
{
  public:
    static const size_t MAX_INSTANCE = 3000u;
    
  protected:
    bool m_bInitialized;
    
    ProgramShader *m_Program;
    InstancedModel *m_Cubes;
  
    size_t m_numInstance;//
  
  public:
    CubeSea()
      : m_bInitialized(false),
        m_Program(0),
        m_Cubes(0),
        m_numInstance(MAX_INSTANCE)
    {}
    
    ~CubeSea();
    
    void init();
    
    void update(/*float dt*/) {}
    
    void render(const Camera& camera);
    
  protected:
    void _setTransformations();
};

#endif //SCENE_CUBESEA_HPP
