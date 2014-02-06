
#pragma once

#ifndef POSTPROCESS_GLOBALPARAMS_H
#define POSTPROCESS_GLOBALPARAMS_H

class Texture2D;

/** parameters that can be used in more than one effect */
struct globalParams_t
{
  // use a RessourceManager instead ?
  Texture2D *pSceneTex;//
  
  Texture2D *pDepthTex;//
  Texture2D *pNormalTex;//
  Texture2D *pMotionTex;//
  
  Texture2D *pInputTex;//
  Texture2D *pAOTex;//
  Texture2D *pBloomTex;//
  Texture2D *pDoFTex;//
    
  float time;
  float rand;
  glm::vec2 linearParams;
  glm::vec2 viewport;
  
  
  globalParams_t()
    : pSceneTex(0), 
      pDepthTex(0), pNormalTex(0), pMotionTex(0),
      pInputTex(0), pAOTex(0), pBloomTex(0), pDoFTex(0),
      time(0.0f), rand(0.0f)
  {}
  
  /** Reset texture slot */
  void reset()
  {
    pInputTex = pAOTex = pBloomTex = pDoFTex = 0;
  }
};


#endif //POSTPROCESS_GLOBALPARAMS_H
