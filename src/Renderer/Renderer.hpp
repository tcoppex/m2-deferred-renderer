/**
 * 
 *    \file Renderer.hpp
 * 
 */


#pragma once

#ifndef RENDERER_RENDERER_HPP
#define RENDERER_RENDERER_HPP


#include <GL/glew.h>
#include <GLType/ProgramShader.hpp>
#include <GLType/RenderTarget/FrameBuffer.hpp>
#include <GLType/Texture.hpp>

class Scene;
class PostProcess;


/**
 * \enum GBufferID
 * 
 * Defines the available G-Buffers' id
 */
enum GBufferID
{
  GB_DIFFUSE = 0,
  GB_NORMAL,
  GB_MOTIONDOF,
  
  NUM_GBUFFER
};

/**
 * \enum GBufferTexUnit
 * 
 * Defines the G-Buffers' texture unit.
 * It is basically the same as GBufferID but the addition of the
 * DEPTH_STENCIL buffer wich is not treat as a standard G-Buffer.
 */
enum GBufferTexUnit
{
  TEXUNIT_DIFFUSE=0,        /** RGB Diffuse + A Specular*/
  TEXUNIT_NORMAL,           /** RGB Normal */
  TEXUNIT_MOTIONDOF,        /** RG MOTION-XY + (todo) B DoF factor */
  
  TEXUNIT_DEPTHSTENCIL,     /** 24 Depth + 8 Stencil */
    
  NUM_TEXUNIT
};


class Renderer
{
  protected:
    bool m_bInitialized;
    
    Scene *m_Scene;     // external ?
    
    PostProcess *m_PostProcess;    
    
    ProgramShader m_fillBufferPS;  // tmp
    ProgramShader m_deferredLightPS;
        
    FrameBuffer m_fillBufferFB;
    FrameBuffer m_sceneFB;
    
    Texture2D m_gBufferTex[NUM_GBUFFER];      /** G-Buffers textures */
    Texture2D m_depthStencilTex;              /** Depth-Stencil buffer texture */
    Texture2D m_sceneTex;                     /** Final Scene buffer texture */
    
    /// To update buffers resolution when viewport changes
    GLint m_width;
    GLint m_height;
    
  
  public:
    Renderer();
    ~Renderer();
    
    void init();    
    void run();    
  
  protected:
    void _renderGeometry();       /** Fill G-Buffers (with geometry) */
    //void _renderDecals();       /** Fill G-Buffers (with decals) */
    void _deferredLightsPass();   /** Compute Lights contribution */
    
    void _initShaders();
    void _initRenderer();
    
    void _configureRenderer();  
};


#endif //RENDERER_RENDERER_HPP
