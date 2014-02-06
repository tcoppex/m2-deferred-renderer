  
  
#include "Image.hpp"



bool Image_t::bINITIALISED = false;

  
Image_t::~Image_t()
{ 
  clean();
}
  
void Image_t::clean()
{
  
  if (imgId)
  {
    ilDeleteImages( 1u, &imgId);
    imgId = 0u;
    data = 0;
  }
  
  
  if (data != 0)
  {
    delete [] data;
    data = 0;
  }
  
}
  
bool Image_t::load(const char *filename)
{
	ilGenImages( 1, &imgId);

  ilBindImage( imgId );
  {
    ILboolean success = ilLoadImage(filename);
    
    assert( success );
    
    if (ilGetInteger(IL_IMAGE_BPP)<=3u) {
      ilConvertImage( IL_RGB, IL_UNSIGNED_BYTE);//
    } else {
      ilConvertImage( IL_RGBA, IL_UNSIGNED_BYTE);//
    }
     
    _setDefaultAttributes();    
  }
  ilBindImage( 0u );
  
  return true;
}

bool Image_t::_setDefaultAttributes()
{  
  bytesPerPixel = ilGetInteger(IL_IMAGE_BPP);
  
  /// bpp works, but I prefers specify a GL constant.. that's cleaner !!
  switch (bytesPerPixel)
  {      
    case 1u:
      internalFormat = GL_RED;
    break;
    
    case 2u:
      internalFormat = GL_RG;
    break;
    
    case 3u:
      internalFormat = GL_RGB;
    break;
    
    case 4u:
      internalFormat = GL_RGBA;
    break;
    
    default:
      assert( "Image_t: invalid bpp ?" && 0 );
  }
  
  format = ilGetInteger(IL_IMAGE_FORMAT);
  
  width = ilGetInteger(IL_IMAGE_WIDTH);
  height = ilGetInteger(IL_IMAGE_HEIGHT);
  target = GL_TEXTURE_2D;//(width==height)? GL_TEXTURE_2D : GL_TEXTURE_RECTANGLE;//
  type = GL_UNSIGNED_BYTE;
  
  data = ilGetData(); 
  
  return true;
}
