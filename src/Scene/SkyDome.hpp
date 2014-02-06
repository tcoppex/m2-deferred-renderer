/**
 * 
 *    \file SkyDome.hpp
 * 
 *  Generate a dome with moving clouds (2 layers).
 * 
 *  Clouds speed and sky color can be change interactively.
 */
 
#pragma once

#ifndef SCENE_SKYDOME_HPP
#define SCENE_SKYDOME_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>

class Camera;
class ProgramShader;
class Mesh;
class Texture2D;


class SkyDome
{
  protected:
    bool m_bInitialized;
    
    ProgramShader *m_Program;
    Mesh *m_Dome;
    Texture2D *m_CloudTex;
                
    glm::mat4 m_scaleMatrix;//
    glm::vec3 m_color;
    float m_cloudSpeed;
    
    
  public:
    SkyDome()
      : m_bInitialized(false),
        m_Program(0),
        m_Dome(0),
        m_CloudTex(0)
    {
      m_color = glm::vec3( 0.5f, 0.75f, 1.0f);
      m_cloudSpeed = 0.0025f;
    }
    
    ~SkyDome();    
    
    void init();
    void render(const Camera& camera);
    
    void setColor(const glm::vec3& color) { m_color = color; }
    void setCloudSpeed(float speed) { m_cloudSpeed = speed; }
    
    const glm::vec3& getColor() { return m_color; }
    float getCloudSpeed() { return m_cloudSpeed; }
    
    
  protected:
    void _initTextures();    
    void _updateUniforms(const Camera& camera);
};

#endif //SCENE_SKYDOME_HPP
