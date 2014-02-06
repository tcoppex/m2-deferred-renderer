/**
 * 
 *    \file Renderer.cpp
 * 
 * 
 */
 

#include "Renderer.hpp"
#include "PostProcess.hpp"
#include <Scene/Scene.hpp>
#include <tools/gltools.hpp>
#include <tools/AppConfig.hpp>
#include <tools/Camera.hpp>
#include <GL/glew.h>



Renderer::Renderer()
  : m_bInitialized(false),
    m_Scene(0),
    m_PostProcess(0),
    m_width(0),
    m_height(0)
{
}

Renderer::~Renderer()
{
  // [automatic]
  {
    m_fillBufferPS.destroy();
    m_deferredLightPS.destroy();
        
    m_fillBufferFB.destroy();
    m_sceneFB.destroy();
    
    for (int i=0; i<NUM_GBUFFER; ++i) {
      m_gBufferTex[i].destroy();
    }
    
    m_depthStencilTex.destroy();    
    m_sceneTex.destroy();
  }
  
  if (m_Scene) delete m_Scene;
  if (m_PostProcess) delete m_PostProcess;
}


void Renderer::init()
{
  assert( !m_bInitialized );
    
  _initShaders();
  _initRenderer();
  
  /// The Renderer holds the scene 'manager' (~)
  m_Scene = new Scene();
  m_Scene->init();  
  
  /// Post-Process initialization
  m_PostProcess = new PostProcess();
  m_PostProcess->init();
  
  // Sets G-Buffer used by post-processing effects
  // xxx not satisfied with the method xxx TODO
  m_PostProcess->setSceneTex( &m_sceneTex );
  m_PostProcess->setDepthTex( &m_depthStencilTex );//
  m_PostProcess->setNormalTex( &m_gBufferTex[GB_NORMAL] );//
  m_PostProcess->setMotionTex( &m_gBufferTex[GB_MOTIONDOF] );//
  
  //note: m_gBufferTex[GB_DIFFUSE] could be used as a postProcess buffer to save memory.
  
  m_bInitialized = true;
}


void Renderer::run()
{ 
  /// 0 - Update / Pre-Processing pass    
  m_Scene->update();
  
  _configureRenderer();//
  
  /// 1 - [DEFERRED] Fill G-buffers
  m_fillBufferFB.begin();
  {
    /// a) Geometry
    _renderGeometry();

    /// b) Decals
    //_renderDecals();
  }
  m_fillBufferFB.end();
  
  
  /// Deferred light composition & forward pass, uses the same DEPTH-STENCIL buffer.
  m_sceneFB.begin();
  {
    /// 2 - [DEFERRED] Compute lights effect pass
    _deferredLightsPass();
    
    /// 3 - [FORWARD] render blended objects / effects
    /**
     *  Render back to front blended objects/effects
     *  with simplified lighting (eg. per vertex, less lights, no shadows, ..etc)
     *  Can render to a lower resolution with MSAA (using an other FBO)..
     */
    //_forwardPass();
    //_renderParticles();
  }
  m_sceneFB.end();
  
  
  /// 4 - Post-Processing  
  m_PostProcess->run();
  
  
  CHECKGLERROR();
}


void Renderer::_renderGeometry()
{
/**
 * This pass fill the G-buffers with all geometry data.
 * For example depth, diffuse, specular, normals, motion,..
 * 
 * This pass can also be used to initialize 'light accumulation
 * buffer' with pre-baked lightmap or ambient light for example.
 * 
 * Shadowmap can be computed too, but it may be preferable to
 * wait for the light pass to test if lights contributes to
 * screen, and then calculate them if needed.
 */


  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
  /**
   * Actually, each object of the "scene graph" will
   * activate its 'fill buffer' program, but here for simplicity and to
   * try this technique out, I use the same program for the whole scene.
   */
  m_fillBufferPS.bind();
  {
    /* The per-Object vertexShader's uniforms are given by the objects */
    m_Scene->render( RENDER_GEOMETRY );  
  }
  m_fillBufferPS.unbind();
    
  
  CHECKGLERROR();
}



void Renderer::_deferredLightsPass()
{ 
/**
 * The light pass is used to accumulate lights into buffers.
 *   
 */
 
  /// STENCIL will be cleared for each lightShape, DEPTH is kept intact.
  glClear( GL_COLOR_BUFFER_BIT );
  
  /// The DEPTH buffer is used, but not written.
  glDepthMask( GL_FALSE ); // move in render ?
    
  /// LightShapes must be rendered filled.
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL); // move in render ?
  
      
  /// Attach the G-buffer textures.
  for (int unit=0; unit<NUM_GBUFFER; ++unit) {
    m_gBufferTex[unit].bind( unit );
  }
  m_depthStencilTex.bind( TEXUNIT_DEPTHSTENCIL );
  
  m_deferredLightPS.bind();
  {
    /// G-Buffers' unit
    m_deferredLightPS.setUniform( "uDiffuseBuffer", TEXUNIT_DIFFUSE);
    m_deferredLightPS.setUniform( "uNormalBuffer", TEXUNIT_NORMAL);
    m_deferredLightPS.setUniform( "uDepthBuffer", TEXUNIT_DEPTHSTENCIL);
    // MOTIONDOF not (yet?) used in this pass
    
    /// Projection parameters to linearize the depth buffer
    const Camera &camera = Camera::GetInstance();
    m_deferredLightPS.setUniform( "uLinearParams", camera.getLinearizationParams());
  
    /// Blending occurs in the rendering function.
    m_Scene->render( RENDER_LIGHTSHAPE );

    /// Just to see the geometry if needed.
    if (AppConfig::GetInstance().getBool( "displayLightShape:Enable" )) {
      m_Scene->render( RENDER_LIGHTGEOMETRY );
    }
  }
  m_deferredLightPS.unbind();
  
  
  /// Dettach the G-buffer textures (reverse order).
  m_depthStencilTex.unbind( TEXUNIT_DEPTHSTENCIL );
  for (int unit=NUM_GBUFFER-1; unit>=0; --unit) {
    Texture::Unbind( GL_TEXTURE_2D, unit );
  }  
    
  glDepthMask( GL_TRUE );
  
  CHECKGLERROR();
}



//----------------------------------------------


void Renderer::_initShaders()
{
  //--
  /// [TEMP] Generic Program used to fill G-buffers.
  m_fillBufferPS.generate();
    m_fillBufferPS.addShader( GL_VERTEX_SHADER, "DR_Buffering.Vertex");
    m_fillBufferPS.addShader( GL_FRAGMENT_SHADER, "DR_Buffering.Fragment");
  m_fillBufferPS.link();
  //--
  
  /// Program used to accumulate lights for the Deferred Rendering.
  m_deferredLightPS.generate();
    m_deferredLightPS.addShader( GL_VERTEX_SHADER, "DR_Lighting.Vertex");
    m_deferredLightPS.addShader( GL_FRAGMENT_SHADER, "DR_Lighting.Fragment");
  m_deferredLightPS.link();    

  CHECKGLERROR();
}



void Renderer::_initRenderer()
{
  /// G-Buffer textures
  m_gBufferTex[GB_DIFFUSE].generate();    
  m_gBufferTex[GB_DIFFUSE].bind();
  Texture2D::DefaultParameters();
  
  m_gBufferTex[GB_NORMAL].generate();    
  m_gBufferTex[GB_NORMAL].bind();
  Texture2D::DefaultParameters(); 
  
  m_gBufferTex[GB_MOTIONDOF].generate();    
  m_gBufferTex[GB_MOTIONDOF].bind();
  Texture2D::DefaultParameters(); 
 
  /// Depth-Stencil texture
  m_depthStencilTex.generate();
  m_depthStencilTex.bind();
  Texture2D::DefaultParameters();
  
  /// Scene texture
  m_sceneTex.generate();    
  m_sceneTex.bind();
  Texture2D::DefaultParameters();
  
  Texture::Unbind( GL_TEXTURE_2D );
   
  
  //-------------
  
  
  /// (Re)Initialize buffers' size.
  _configureRenderer();
   
  
  //-------------
  
  
  /// FrameBuffer storing G-Buffers
  m_fillBufferFB.generate();
  m_fillBufferFB.bind();
  {  
    /// Attach G-buffer textures.   
    m_fillBufferFB.attachColorBuffer( GL_COLOR_ATTACHMENT0, &(m_gBufferTex[GB_DIFFUSE]));
    m_fillBufferFB.attachColorBuffer( GL_COLOR_ATTACHMENT1, &(m_gBufferTex[GB_NORMAL])); 
    m_fillBufferFB.attachColorBuffer( GL_COLOR_ATTACHMENT2, &(m_gBufferTex[GB_MOTIONDOF]));
    
    /// Depth-Stencil, for Depth testing & to use as a write G-buffer.
    m_fillBufferFB.attachSpecialBuffer( GL_DEPTH_STENCIL_ATTACHMENT, &(m_depthStencilTex));
    
    m_fillBufferFB.complete(); 
  }
  m_fillBufferFB.unbind();
  
  /// FrameBuffer storing final scene composition
  m_sceneFB.generate();
  m_sceneFB.bind();
  { 
    m_sceneFB.attachColorBuffer( GL_COLOR_ATTACHMENT0, &(m_sceneTex));
    
    /// Use the same Depth-Stencil, for Depth-Stencil test & to use as a read G-buffer.
    m_sceneFB.attachSpecialBuffer( GL_DEPTH_STENCIL_ATTACHMENT, &(m_depthStencilTex));
    
    m_sceneFB.complete(); 
  }
  m_sceneFB.unbind();


  CHECKGLERROR();
}


void Renderer::_configureRenderer()
{  
  GLint vp[4];  
  
  /// Retrieve screen resolution
  glGetIntegerv( GL_VIEWPORT, vp);
    
  /// Update the texture resolution only if it has changed
  if ((m_width == vp[2]) && (m_height == vp[3])) {
    return;
  }  
  m_width = vp[2];
  m_height = vp[3];
   
  
  //-------------
  
  
  /// not really needed here (just for the fbo completion)
  m_fillBufferFB.resize( m_width, m_height); //
  m_sceneFB.resize( m_width, m_height); //
  
  
  //-------------
  
  
  /**
   * for texture format, cf. OpenGL reference v4.1 p220
   * RGBA16F is used to represented unormalized floating point
   */
   
  /// [RGB Diffuse + A Specular]
  m_gBufferTex[GB_DIFFUSE].bind();
  Texture::Image2D( GL_RGBA8, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  
  /// [RGB XYZ-normal] 
  // TODO : RG XY-normal & compute Z in a shader -some bug with it for the moment-
  m_gBufferTex[GB_NORMAL].bind();
  Texture::Image2D( GL_RGB16F, m_width, m_height, GL_RGB, GL_FLOAT, 0);
  
  /// [RG XY-motion + B DoF] (too big ?)
  m_gBufferTex[GB_MOTIONDOF].bind();
  Texture::Image2D( GL_RGB16F, m_width, m_height, GL_RGB, GL_FLOAT, 0);
  
  
  /// [Depth24 + Stencil8]
  m_depthStencilTex.bind();
  Texture::Image2D( GL_DEPTH24_STENCIL8, m_width, m_height, GL_DEPTH_STENCIL, 
                    GL_UNSIGNED_INT_24_8, 0); // format is important !!!
  
  /// Scene texture
  m_sceneTex.bind();
  Texture::Image2D( GL_RGBA8, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, 0);

  // Unbind texture
  Texture::Unbind( GL_TEXTURE_2D );
    
  
  CHECKGLERROR();
}
