/**
 * 
 *    \file App.hpp  
 * 
 */
 

#pragma once

#ifndef APP_HPP
#define APP_HPP

#include <tools/Camera.hpp>
class Renderer;

class App
{
  public:
    static const int DEFAULT_WIDTH = 1280u;
    static const int DEFAULT_HEIGHT = 720u;    
    static int ScreenWidth;//
    static int ScreenHeight;//
    
  protected:
    bool m_bWireframe;
    bool m_bFullscreen;
  
    Camera &m_rCamera;
    Renderer *m_Renderer;
    //Scene *m_Scene;
    
  
  public:
    App();
    ~App();
    
    void init( int argc, char *argv[]);    
    
    // GLUT functions handler
    void reshape(int, int);    
    void display();
    void keyboard( unsigned char, int, int);
    void special( int, int, int);    
    void specialUp( int, int, int);    
    void mouse(int, int, int, int);
    void motion(int, int);
    void idle();
    
  protected:
    void _initExtension();
    void _initGL();
    void _initConfig();
    
    void _update();
    void _cameraKeys(int key, bool isPressed);
    
    void _destroySingletonInstances();
};


#endif //APP_HPP
