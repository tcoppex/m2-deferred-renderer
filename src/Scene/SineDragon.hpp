/**
 * 
 *  \file SineDragon.hpp
 * 
 *  Set of instanced cube controlled by trigonometric function
 *  to appear like a dragon
 */
 

#ifndef SCENE_SINEDRAGON_HPP
#define SCENE_SINEDRAGON_HPP

#include <cstdlib>
class ProgramShader;
class InstancedModel;
class Camera;


class SineDragon
{
  public:
    static const size_t MAX_INSTANCE = 3000u;
    
  protected:
    bool m_bInitialized;
    
    /** Generic render pass into G-buffer */
    ProgramShader *m_Program;
    
    /** Motion-Blur prepass */
    ProgramShader *m_MotionBlurPS;
    
    InstancedModel *m_DragonBody;    
  
    size_t m_numInstance;//
  
  public:
    SineDragon()
      : m_bInitialized(false),
        m_Program(0),
        m_MotionBlurPS(0),
        m_DragonBody(0),
        m_numInstance(MAX_INSTANCE)
    {}
    
    ~SineDragon();
    
    
    /** */
    void init();
    
    void update(/*float dt*/);
    
    /** */
    void render(const Camera& camera);
    
  protected:
    void _initMatrices();
    
};

#endif //SCENE_SINEDRAGON_HPP
