/**
 * 
 *    \file App.cpp
 * 
 */


#include <GL/glew.h>

#if defined(__APPLE__) || defined(MACOSX)
  #include <GLUT/glut.h>
#else
  #include <GL/freeglut.h>
#endif

#include <glm/glm.hpp>
#include <glsw/glsw.h>

#include <tools/AppConfig.hpp>
#include <tools/Logger.hpp>
#include <tools/Timer.hpp>
#include <Renderer/Renderer.hpp>
#include "App.hpp"



int App::ScreenWidth = DEFAULT_WIDTH;
int App::ScreenHeight = DEFAULT_HEIGHT;


App::App()
  : m_bWireframe(false),
    m_bFullscreen(false),
    m_rCamera(Camera::GetInstance()),
    m_Renderer(NULL)
{
}

App::~App()
{
  glswShutdown();    
  Logger::GetInstance().close();
  
  _destroySingletonInstances();
}


void App::init(int argc, char *argv[])
{  
  /// Random Number Generator
  srand( getpid() );  
  
  /// OpenGL extensions  
  _initExtension();
  
  /// OpenGL params
  _initGL();
  
  /// Application parameters
  _initConfig();
  
  /// GLSW, shader file manager
  glswInit();
  glswSetPath("./shaders/", ".glsl");
  glswAddDirectiveToken("*", "#version 330 core");

  /// Camera
  glm::vec3 eye( 0.0f, 5.0f, 0.0f);
  glm::vec3 dir( 1.0f, 0.0f, 0.0f);
  m_rCamera.setViewParams( eye, 
                           eye+dir );
  m_rCamera.setMoveCoefficient(0.35f);
  
  /// Misc
  Timer::GetInstance().start();
  //Logger::GetInstance().open( "logfile" );
  
  m_Renderer = new Renderer();
  m_Renderer->init();
}

void App::_initExtension()
{
  glewExperimental = GL_TRUE;

  GLenum result = glewInit(); 
  if (result != GLEW_OK)
  {
    fprintf( stderr, "Error: %s\n", glewGetErrorString(result));
    exit( EXIT_FAILURE );
  }

  fprintf( stderr, "GLEW version : %s\n", glewGetString(GLEW_VERSION));

  //assert( glewIsSupported("") );
}

void App::_initGL()
{
  /// Clear the error buffer (it starts with an error)
  glGetError();

  /// Display the version used, not the one available
  fprintf( stderr, "OpenGL version : %s\n", glGetString(GL_VERSION));

  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f);
      
  glEnable( GL_DEPTH_TEST );
  glDepthFunc( GL_LEQUAL );
      
  glDisable( GL_STENCIL_TEST );
  glClearStencil( 0 );

  glDisable( GL_CULL_FACE );
  glCullFace( GL_BACK );    
  glFrontFace(GL_CCW);

  glDisable( GL_MULTISAMPLE );
}

void App::_initConfig()
{
  AppConfig &config = AppConfig::GetInstance();
  
  config.setBool( "PostProcessing:Enable", true);//
  config.setBool( "ppSSAO:Enable", false);
  config.setBool( "ppBloom:Enable", false);
  config.setBool( "ppDoF:Enable", false);//
  config.setBool( "ppMotionBlur:Enable", false);
  config.setBool( "ppVignetting:Enable", false);
  config.setBool( "ppGammaCorrection:Enable", false);
  
  config.setFloat( "Gamma", 2.2f);
  
  
  config.setBool( "Dragon:Enable", false);  
  config.setBool( "Water:Enable", false); 
  
  config.setBool( "rideDragon:Enable", false);  
  config.setBool( "displayLightShape:Enable", false);
    
  // TODO..
}


void App::_update()
{  
  m_rCamera.update();
  Timer::GetInstance().update();
}



/* --- GLUT EVENT HANDLERS --- */

void App::reshape( int w, int h)
{
  glViewport( 0, 0, w, h);
  
  float aspectRatio = ((float)w) / ((float)h);
  m_rCamera.setProjectionParams( 60.0f, aspectRatio, 0.1f, 500.0f);
  
  ScreenWidth = w;
  ScreenHeight = h;
}


void App::display()
{
  _update();
  
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );//
  glPolygonMode(GL_FRONT_AND_BACK, (m_bWireframe)? GL_LINE : GL_FILL);//
  
  m_Renderer->run();

  glutSwapBuffers();
}


void App::keyboard( unsigned char key, int x, int y)
{
  AppConfig &config = AppConfig::GetInstance();
  
  switch (key)
  {
    case 27:
      exit( EXIT_SUCCESS );
    
    case 'w':
      m_bWireframe = !m_bWireframe;
    break;
    
    case 't':
    {
      Timer &timer = Timer::GetInstance();
      printf( "fps : %d [%.3f ms]\n", timer.getFPS(), timer.getElapsedTime());
    }
    break;
    
    case 'x':
    {
      glm::vec3 pos = m_rCamera.getPosition();
      printf( "%f %f %f\n", pos.x, pos.y, pos.z);
    }
    break;
    /*
    case 'f':      
      m_bFullscreen = !m_bFullscreen;
      
      if (m_bFullscreen) {
        glutFullScreen();
      } else {
        glutReshapeWindow( App::DEFAULT_WIDTH, App::DEFAULT_HEIGHT);
      }
    break;
    */
    
    case 'r':
      config.toggle( "rideDragon:Enable" );
    break;
    
    case 'l':
      config.toggle( "displayLightShape:Enable" );
    break;
    
    default:
      break;
  }
}

void App::special( int key, int x, int y)
{
  _cameraKeys( key, true);
  
  
  AppConfig &config = AppConfig::GetInstance();
  
  switch (key)
  {
    case 1: config.toggle( "Dragon:Enable" );  
    break;
    
    case 2: config.toggle( "Water:Enable" );
    break;
    
    
    case 3: config.toggle( "ppSSAO:Enable" );
    break;
    
    case 4: config.toggle( "ppBloom:Enable" );
    break;
    
    case 5: config.toggle( "ppMotionBlur:Enable" );
    break;
    
    case 6: config.toggle( "ppVignetting:Enable" );
    break;
    
    case 7: config.toggle( "ppGammaCorrection:Enable" );
    break;
  }
}

void App::specialUp( int key, int x, int y)
{
  _cameraKeys( key, false);
}

void App::mouse(int button, int state, int x, int y)
{  
  if (state == GLUT_DOWN) 
  { 
    m_rCamera.motionHandler( x, y, true); 
  }
}

void App::motion(int x, int y)
{
  m_rCamera.motionHandler( x, y, false);
}

void App::idle()
{
  //glutWarpPointer( WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
}


/* --- --- --- */


void App::_cameraKeys(int key, bool isPressed)
{
  CameraKeys ckey;
  
  switch (key)
  {
    case GLUT_KEY_UP:
      ckey = MOVE_FORWARD;
    break;
    
    case GLUT_KEY_DOWN:
      ckey = MOVE_BACKWARD;
    break;
    
    case GLUT_KEY_LEFT:
      ckey = MOVE_LEFT;
    break;
    
    case GLUT_KEY_RIGHT:
      ckey = MOVE_RIGHT;
    break;
    
    default:
      ckey = NO_CAMERAKEYS;
    break;
  }
  
  if (ckey != NO_CAMERAKEYS) {
    m_rCamera.keyboardHandler( ckey, isPressed);
  }
}


void App::_destroySingletonInstances()
{
  AppConfig::DeleteInstance();
  Camera::DeleteInstance();
  Logger::DeleteInstance();
  Timer::DeleteInstance();
}
