/**
 * 
 *  \file SponzaModel.hpp
 * 
 *  This is a really quick file to load the sponza scene
 *  it was not think to be clean or to fit perfectly with the rest of the engine..
 * 
 *  (but it's not so wasted, it can be cleaned)
 * 
 *  Actually, it is so well done that it could load any kind of models
 *  (supposing textures internal data are the same).
 */


#ifndef SCENE_SPONZAMODEL_HPP
#define SCENE_SPONZAMODEL_HPP

#include <GLType/VertexBuffer.hpp>
#include <vector>
#include <string>
#include <map>

class Camera;
class ProgramShader;
class Texture2D;
class aiMesh;
class aiMaterial;


struct Mesh_t
{
  VertexBuffer vbuffer;
  unsigned int materialIndex;
  
  Mesh_t() : 
    materialIndex(0u)
  {}
};

struct Material_t
{
  //float Ns;
	//float Ni;
	//float d;
	//float Tr;
	//float Tf;
	//int illum;
	//glm::vec3 Ka;
	glm::vec3 Kd;
	glm::vec3 Ks;
	//glm::vec3 Ke;	
  //Texture2D *pSpecularMap;
  Texture2D *pDiffuseMap;
  Texture2D *pNormalMap;
  
  Material_t() :
    //Ns(0.0f), Ni(1.0f), d(1.0f), Tr(0.0f), Tf(0.0f), illum(0),
    //Ka(glm::vec3(0.0f)), 
    Kd(glm::vec3(1.0f)), Ks(glm::vec3(0.0f)), //Ke(glm::vec3(0.0f)), 
    //pSpecularMap(0), 
    pDiffuseMap(0), pNormalMap(0)
  {}
};

class SponzaModel
{
  protected:    
    bool m_bLoaded;
    std::string m_fileroot;    
    float m_scale;
    
    typedef std::vector<Mesh_t> MeshArrays;    
    MeshArrays m_meshes;
    
    typedef std::vector<Material_t> MaterialArrays;
    MaterialArrays m_materials;
    
    typedef std::map<std::string, Texture2D*> TextureMap_t;
    TextureMap_t m_textures;
    
    ProgramShader *m_Program;
    
    
  public:
    SponzaModel() 
      : m_bLoaded(false),
        m_scale(1.0f/64.0f),
        m_Program(0)
    {}
    
    ~SponzaModel();
    
    void init();    
    void render( const Camera& camera );    
    
  protected:
    void _loadScene();
    
    void _loadMesh(const aiMesh &dataMesh, int idMesh);
    void _loadMaterial( const aiMaterial &dataMaterial, int idMat);
    
    Texture2D* _loadTexture( const char *name, bool isBump );
    
    void _renderMesh(int idMesh);
};

#endif //SCENE_SPONZAMODEL_HPP
