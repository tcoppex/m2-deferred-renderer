/**
 *             
 *                \file main.cpp
 * 
 * 
 */


#ifdef _WIN32
  #include <windows.h>
#endif

// GLUT (GL Utility Toolkit)
#if defined(__APPLE__) || defined(MACOSX)
  #include <GLUT/glut.h>
#else
  #include <GL/freeglut.h>
#endif

#include "App.hpp"



namespace
{
  const char* WINDOW_NAME = "Deferred Renderer";
  
  App app;
  int glut_windowHandle = 0;  
    
  //~
  
  void initWM( int argc, char *argv[]);  
  
  void glut_reshape_callback(int, int);    
  void glut_display_callback();
  void glut_keyboard_callback( unsigned char, int, int);
  void glut_special_callback( int, int, int);    
  void glut_specialUp_callback( int, int, int);    
  void glut_mouse_callback(int, int, int, int);
  void glut_motion_callback(int, int);
  void glut_idle_callback();
}


int main(int argc, char *argv[])
{
  // Initialize the window manager (GLUT)
  initWM( argc, argv);
  
  // Initialize the application
  app.init( argc, argv); 
  
  // note: the mainloop returns
  glutMainLoop();  
  
  return EXIT_SUCCESS;
}


namespace
{
  void initWM( int argc, char *argv[])
  {
    glutInit( &argc, argv);
    
    // OpenGL profile to use
    glutInitContextVersion( 3, 3);
    glutInitContextFlags( GLUT_FORWARD_COMPATIBLE );
    glutInitContextProfile( GLUT_CORE_PROFILE );
    
    // Set 'glutMainLoop' to return
    glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS );
    
    glutInitWindowSize( App::DEFAULT_WIDTH, App::DEFAULT_HEIGHT);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL );
    glut_windowHandle = glutCreateWindow( WINDOW_NAME );
    
    if (glut_windowHandle < 1)
    {
      fprintf( stderr, "Error: window creation failed.\n");
      exit( EXIT_FAILURE );
    }
    
    glutReshapeFunc( glut_reshape_callback );
    glutDisplayFunc( glut_display_callback );
    glutKeyboardFunc( glut_keyboard_callback );
    glutSpecialFunc( glut_special_callback );
    glutSpecialUpFunc( glut_specialUp_callback );
    glutMouseFunc( glut_mouse_callback );
    glutMotionFunc( glut_motion_callback );
    glutIdleFunc( glut_idle_callback );    
  }
  
  void glut_reshape_callback(int w, int h)
  {
    app.reshape(w, h);
  }
  
  void glut_display_callback()
  {
    app.display();
  }
  
  void glut_keyboard_callback(unsigned char key, int x, int y)
  {
    app.keyboard( key, x, y);    
  }
  
  void glut_special_callback(int key, int x, int y)
  {
    app.special( key, x, y);
  }
  
  void glut_specialUp_callback(int key, int x, int y)
  {
    app.specialUp( key, x, y);
  }
  
  void glut_motion_callback(int x, int y)
  {
    app.motion( x, y);
    glutPostRedisplay();
  }
  
  void glut_mouse_callback(int button, int state, int x, int y)
  {
    app.mouse( button, state, x, y);
  }
  
  void glut_idle_callback()
  {
    app.idle();
    glutPostRedisplay();
  }
}
