

#include "SponzaModel.hpp"
#include <GLType/Texture.hpp>
#include <GLType/ProgramShader.hpp>
#include <tools/Camera.hpp>
#include <tools/gltools.hpp>
#include <assimp/assimp.hpp>      // C++ importer interface
#include <assimp/aiScene.h>       // Output data structure
#include <assimp/aiPostProcess.h> // Post processing flags


SponzaModel::~SponzaModel()
{
  TextureMap_t::iterator it;
  for (it=m_textures.begin(); it!=m_textures.end(); ++it) {
    delete it->second;
  }
}
    
void SponzaModel::init()
{  
  _loadScene();
  m_bLoaded = true;
    
  m_Program = new ProgramShader();
  m_Program->generate();
    m_Program->addShader( GL_VERTEX_SHADER, "Advanced.Vertex" );
    m_Program->addShader( GL_FRAGMENT_SHADER, "Advanced.Fragment" );
  m_Program->link();
}
    
void SponzaModel::render( const Camera& camera )
{
  if (!m_bLoaded) {
    return;
  }
  
  assert( m_meshes.empty() == false );
  
  const glm::mat4 scale = glm::scale( glm::mat4(1.0f), glm::vec3(m_scale));
  const glm::mat4 mvp = camera.getViewProjMatrix() * scale;
  
  /// don't use the scaling matrix. Don't needs to, it's a zoom.
  const glm::mat3 normalView = glm::mat3( camera.getViewMatrix() );
    
  m_Program->bind();
  {
    m_Program->setUniform( "uModelViewProjMatrix", mvp);
    m_Program->setUniform( "uNormalViewMatrix", normalView);
    
    // TODO: frustum culling !
    for (int i=0; i<m_meshes.size(); ++i) {
      _renderMesh(i);
    }
  }
  m_Program->unbind();
}

void SponzaModel::_loadScene()
{
  Assimp::Importer importer;
  
  
  const char *objfile = "./datas/crytek-sponza/sponza.obj";
  
  /// Retrieve object root directory (XXX system specific)
  m_fileroot = std::string(objfile);
  m_fileroot.resize( m_fileroot.find_last_of('/')+1u );  
  
  const aiScene* scene = importer.ReadFile( objfile,
        aiProcess_GenSmoothNormals        |
        aiProcess_CalcTangentSpace        |   /// used for Normal Mapping
        aiProcess_Triangulate             |   /// to have only triangles in the mesh
        aiProcess_JoinIdenticalVertices   |   /// to use indexed geometry
        aiProcess_SortByPType
      );
      
  if (!scene)
  {
    fprintf( stderr, "%s\n", importer.GetErrorString());
    return;
  }
    
  
  /// As every materials properties are not loaded, some redundancy could happens
  m_materials.resize( scene->mNumMaterials );    
  for (int i=0; i<m_materials.size(); ++i) {
    _loadMaterial( *(scene->mMaterials[i]), i);
  }
  
  m_meshes.resize( scene->mNumMeshes );
  for (int i=0; i<m_meshes.size(); ++i) {
    _loadMesh( *(scene->mMeshes[i]), i );
  }
}

void SponzaModel::_loadMesh(const aiMesh &dataMesh, int idMesh)
{ 
  /// Attachs the material id & retrieve the material objects
  m_meshes[idMesh].materialIndex = dataMesh.mMaterialIndex;
  Material_t &mat = m_materials[ dataMesh.mMaterialIndex ];
  
  
  /// Retrieve generics attributes (position, normals & texcoords)
  VertexBuffer &vBuffer = m_meshes[idMesh].vbuffer;
  
  std::vector<glm::vec3> &positions = vBuffer.getPosition();
  std::vector<glm::vec3> &normals   = vBuffer.getNormal();
  std::vector<glm::vec2> &texCoords = vBuffer.getTexcoord();
  
  assert( dataMesh.HasNormals() );
  assert( dataMesh.HasTextureCoords(0u) );
  
  positions.resize( dataMesh.mNumVertices );  
  normals.resize( dataMesh.mNumVertices );
  texCoords.resize( dataMesh.mNumVertices );
  
  for (unsigned int i=0u; i<dataMesh.mNumVertices; ++i)
  {
    const aiVector3D &pos = dataMesh.mVertices[i];
    positions[i] = glm::vec3( pos.x, pos.y, pos.z );
    
    const aiVector3D &nor = dataMesh.mNormals[i];
    normals[i] = glm::vec3( nor.x, nor.y, nor.z );
    
    const aiVector3D &uv = dataMesh.mTextureCoords[0][i];
    texCoords[i] = glm::vec2( uv.x, uv.y);
  }

  /// Retrieve Tangent & Bitangent for bump mapping if needed
  ///(alternatively, they could be computed in the vertex shader)
  #if 1
  if (mat.pNormalMap != 0)
  {    
    assert( dataMesh.HasTangentsAndBitangents() );
    std::vector<glm::vec3> &tangents   = vBuffer.getTangent();
    std::vector<glm::vec3> &bitangents = vBuffer.getBitangent();
    
    tangents.resize( dataMesh.mNumVertices );
    bitangents.resize( dataMesh.mNumVertices );
    
    for (unsigned int i=0u; i<dataMesh.mNumVertices; ++i)
    {
      const aiVector3D &t = dataMesh.mTangents[i];
      tangents[i] = glm::vec3( t.x, t.y, t.z );  
      
      const aiVector3D &bt = dataMesh.mBitangents[i];
      bitangents[i] = glm::vec3( bt.x, bt.y, bt.z );  
    }
  }
  #endif
  
  std::vector<GLuint> &indices = vBuffer.getIndices();
  for (unsigned int i=0u; i<dataMesh.mNumFaces; ++i)
  {
    const aiFace &face = dataMesh.mFaces[i];
    indices.push_back( face.mIndices[0] );
    indices.push_back( face.mIndices[1] );
    indices.push_back( face.mIndices[2] );
  }
  
  vBuffer.complete( GL_STATIC_DRAW );
  vBuffer.cleanData();
}


void SponzaModel::_loadMaterial( const aiMaterial &dataMaterial, int idMat)
{
  Material_t &mat = m_materials[idMat];
  
    
  /// Colors
  aiColor3D color;
  dataMaterial.Get( AI_MATKEY_COLOR_DIFFUSE, color);
  mat.Kd = glm::vec3( color.r, color.g, color.b);
  
  dataMaterial.Get( AI_MATKEY_COLOR_SPECULAR, color);
  mat.Ks = glm::vec3( color.r, color.g, color.b);
    
  ///Textures  
  unsigned int numDiffuse = dataMaterial.GetTextureCount( aiTextureType_DIFFUSE );    
  if (numDiffuse != 0u)
  {
    aiString filename;
    dataMaterial.GetTexture( aiTextureType_DIFFUSE, 0u, &filename);
    mat.pDiffuseMap = _loadTexture( filename.data, false );
  }
  
  unsigned int numBump = dataMaterial.GetTextureCount( aiTextureType_HEIGHT );
  if (numBump != 0u)
  {
    aiString filename;
    dataMaterial.GetTexture( aiTextureType_HEIGHT, 0u, &filename);
    mat.pNormalMap = _loadTexture( filename.data, true );
  }
}

Texture2D* SponzaModel::_loadTexture(const char *name, bool isBump)
{
  /// If the texture has already been loaded, return a pointer to it
  /// The RAW name is used as key to avoid costly reformatting operation
  TextureMap_t::iterator it = m_textures.find(name);  
  if (it != m_textures.end()) {
    return it->second;
  } 
 
  std::string sysName(name);
  
  /// Replace Windows' antislash by Unix' slash
  size_t pos;
  while ( (pos=sysName.find_first_of('\\')) != std::string::npos )
  {
    sysName.replace( pos, 1u, 1u, '/' );
  }
    
  /// Complete filename relative to the application's root
  std::string filename = m_fileroot + sysName;
  
  /// Creates the texture
  Texture2D *pTexture = new Texture2D();
  
  ///Every texture got mipmapping but AnisotropicFilter is only for non bumpMap
  bool bMipmapping = true;
  bool bAnisotropicFilter = !isBump;  
  bool res = pTexture->load(filename, bMipmapping, bAnisotropicFilter);
  
  assert( false != res );
  
  pTexture->bind();
    Texture2D::SetWrapParameters( GL_REPEAT, GL_REPEAT);
  pTexture->unbind();
  
  
  /// Update the textures set
  m_textures[ name ] = pTexture;
  
  printf("%s loaded.\n", filename.c_str());
    
  return pTexture;
}



void SponzaModel::_renderMesh(int idMesh)
{
  const VertexBuffer& vBuffer = m_meshes[idMesh].vbuffer;
  const Material_t& mat = m_materials[ m_meshes[idMesh].materialIndex ];
  
  
  int texUnit=0;
  
  if (mat.pDiffuseMap)
  {
    mat.pDiffuseMap->bind(texUnit);
    m_Program->setUniform( "uDiffuseTex", texUnit++);  
  }
  
  if (mat.pNormalMap)
  {
    mat.pNormalMap->bind(texUnit);
    m_Program->setUniform( "uNormalTex", texUnit++);
    m_Program->setUniform( "uBumpMapping", true);
  }
  else
  {
    m_Program->setUniform( "uBumpMapping", false);
  }
  
  m_Program->setUniform( "uKd", mat.Kd);  
  m_Program->setUniform( "uKs", mat.Ks);  
  
  vBuffer.draw();
  
  /// Unbind textures
  while (texUnit) {
    Texture::Unbind( GL_TEXTURE_2D, --texUnit);
  }
}
