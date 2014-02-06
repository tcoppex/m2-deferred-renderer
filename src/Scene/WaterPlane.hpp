/**
 * 
 *      \file WaterPlane.hpp
 * 
 */
 

#ifndef SCENE_WATERPLANE_HPP
#define SCENE_WATERPLANE_HPP

#include <tools/Camera.hpp>
#include <glm/glm.hpp>
class Model;
class ProgramShader;
class Texture2D;
class FrameBuffer;


class WaterPlane
{
  protected:
    Model *m_Plane; // or PlaneMesh directly..
    ProgramShader *m_Program;
    
    struct waveData_t
    {
      float amplitude;
      float frequency;
      float phase;
      glm::vec2 direction;
    };
    
    static const int MAX_WAVE = 8;
    waveData_t m_waves[MAX_WAVE];
    int m_numWave;
    
    Texture2D *m_ReflectionTex;
    FrameBuffer *m_ReflectionFB;
    
    /** True if the reflection is currently beeing rendered, false otherwise */
    bool m_bRenderingReflection;
    
    /** Color & transparency (not imp yet) of the water */
    glm::vec4 m_color;
    
    // XX XX XX cé pa joli
    //save params
    glm::mat4 m_oldView;
    float m_projFov;
    float m_projZNear;
    float m_projZFar;
    int m_viewport[4];
    // XX XX XX
  
  public:
    WaterPlane()
      : m_Plane(0),
        m_Program(0),
        m_numWave(MAX_WAVE),
        m_ReflectionTex(0),
        m_ReflectionFB(0),
        m_bRenderingReflection(false)
    {}
    
    virtual ~WaterPlane() {_destroy();}
    
    
    void init();
    
    //void update();
    
    void beginReflection(Camera& camera);//
    void endReflection(Camera& camera);//
    
    void render(const Camera& camera);
    
    const glm::vec4& getColor() { return m_color; }
    void setColor(const glm::vec4& color) { m_color = color; }
  
  protected:    
    void _destroy();
    void _initWaves();
    void _initReflectionRT();
    void _setWavesUniforms();
};

#endif //SCENE_WATERPLANE_HPP
