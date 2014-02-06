/**            
 *                \file PostProcess.hpp
 * 
 * 
 *        note:
 *        Initially a single 'DoubleBuffered RenderTexture' object was used.
 *        The two buffers swapped after each effects. But as some effects
 *        needs more than one pass, each effects holds their RenderTexture objects.
 *        Memory can thus be saved, by sharing buffers between some effects.
 * 
 *        
 *        Using a list is questionnable, at first it make sense for the double
 *        buffered RenderTexture (see below), but now we can specify the effects
 *        statically (it can be useful though to load custom effects).
 */


#pragma once

#ifndef RENDERER_POSTPROCESS_HPP
#define RENDERER_POSTPROCESS_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <list>
#include "PostProcessing/globalParams.h"

class PostEffect;

class PostProcess
{
  protected:    
    bool m_bInitialized;    
    
    /** Render the final scene. Always applied. */
    PostEffect *m_FinalEffect;    
    
    /** List of post-processing effects */
    typedef std::list<PostEffect*> postEffectList_t;
    postEffectList_t m_ppEffects;
    
    /** Generics parameters */
    globalParams_t m_globalParams;//
    
    
  public:
    PostProcess();
    ~PostProcess();
        
    void init();    
    void run();
    
    void setSceneTex( Texture2D *pTexture ) { m_globalParams.pSceneTex = pTexture; }//
    void setDepthTex( Texture2D *pTexture ) { m_globalParams.pDepthTex = pTexture; }//
    void setNormalTex( Texture2D *pTexture ) { m_globalParams.pNormalTex = pTexture; }//
    void setMotionTex( Texture2D *pTexture ) { m_globalParams.pMotionTex = pTexture; }//

  protected:
    void _initGlobalParams();
    
    //void _configureRenderer();
  
  private:
    PostProcess(const PostProcess&);
    PostProcess& operator=(const PostProcess&);
};

#endif //RENDERER_POSTPROCESS_HPP
