#include "../Tools/Perlin.h"
#include "TextureUtils.h"
#include <GL/glu.h>

Texture::Texture(GLuint target_) : IOXMLObject("Texture")
{
  currUnit       = -1;
  target         = target_;
  height         = 0;
  width          = 0;
  depth          = 0;
  id             = 0;
}

Texture::Texture(const Texture & texture) : IOXMLObject("Texture")
{
  currUnit       = -1;
  target         = GL_TEXTURE_2D;
  height         = 0;
  width          = 0;
  depth          = 0;
  id             = 0;
  this->operator=(texture);
}

Texture &Texture::operator= (const Texture & copy)
{
  if(this != &copy)
  {
    setID(copy.id);

    currUnit     = copy.currUnit;
    target       = copy.target;
    height       = copy.height;
    width        = copy.width;
    depth        = copy.depth;
  }
  return *this;
}

bool Texture::checkForRepeat(const char* string)
{
  TextureInfo *textureInfo = NULL;

  if(textureInfo = TexturesManager::getTextureInfo(string))
    setID(textureInfo->getMedia());

  return (textureInfo != NULL);
}

bool Texture::finalizeLoading(const char* string)
{
  if(!id)
    return Logger::writeErrorLog(String("Failed to load texture <") + string + ">");
  else
  {
    TextureInfo *textureInfo = new TextureInfo(string, id);
    TexturesManager::addTextureInfo(textureInfo);
  }
  return true;
}

const bool Texture::activate(int unit)
{
  if(!id)
    return false;

  if(unit > -1)
    glActiveTextureARB(GL_TEXTURE0_ARB + unit);

  glEnable(target);
  glBindTexture(target, id);
 
  currUnit = unit;
  return true;
}

void Texture::copyCurrentBuffer(int newTarget)
{
  if(!id)
    return;

  newTarget = (newTarget == -1) ? target : newTarget;
  activate();
  glCopyTexSubImage2D(newTarget, 0, 0, 0, 0, 0, width, height);
  deactivate();
}

const bool Texture::deactivate()
{
  if(id > 0 )
  {
    if(currUnit > -1)
      glActiveTextureARB(GL_TEXTURE0_ARB + currUnit);

    glDisable(target);
    return true;
  }
  return false;
}

void Texture::setID(GLuint texID)
{
  static int unknown = 1;

  if(!texID)
  {
    Logger::writeErrorLog("invalid texID, Texture::setID(GLuint texID)");
    return;
  }

  TextureInfo *newTextureInfo = TexturesManager::getTextureInfo(texID),
              *oldTextureInfo = TexturesManager::getTextureInfo(id);

  if(newTextureInfo)
    newTextureInfo->increaseUserCount();
  else
  {
    newTextureInfo = new TextureInfo(String("Unknown texture #") + unknown, texID);
    unknown++;
  }
  
  if(oldTextureInfo)
    oldTextureInfo->decreaseUserCount();

  id = texID;

  GLint info;

  activate();
  glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH , &info);
  width  = info;
  glGetTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, &info);
  height = info;
  deactivate();
}
void Texture::setTarget(GLuint target_){ target = target_;}
const GLuint Texture::getTarget() const { return target;  }
const GLuint Texture::getHeight() const { return height;  }
const GLuint Texture::getWidth()  const { return width;   }
const GLuint Texture::getDepth()  const { return depth;   } 
const GLuint Texture::getID()     const { return id;      }

bool Texture::loadXMLSettings(const TiXmlElement *element)
{
  if(!isSuitable(element))
    return false;

  String       path; 
  int          clampS    = GL_CLAMP,
               clampT    = GL_CLAMP,
               aniso     =         0,
               magFilter = GL_LINEAR,
               minFilter = GL_LINEAR;
  bool         mipmap    = false,
               result    = false;

  const char *description = element->Attribute("description");
  if(description)
    return  IOXMLObject::loadXMLSettings(description);

  mipmap = XMLArbiter::analyzeBooleanAttr(element, "mipmap", true);
  target = getXMLType(element->Attribute("type"));
  path   = element->Attribute("path");
  
  for(const TiXmlElement *child = element->FirstChildElement();	
      child;
   	  child = child->NextSiblingElement() )
  {
    String childName  = child->Value();

    if(!childName.getLength())
      continue;

    if(childName == "Wrap")
    {
      const char* attribute = child->Attribute("s");
      attribute             = attribute ? attribute :
                              child->Attribute("u");

      clampS  = getXMLWrapMode(attribute);

      attribute = child->Attribute("t");
      attribute = attribute ? attribute : child->Attribute("v");

      clampT  = getXMLWrapMode(attribute);
      continue;
    }

    if(childName == "Filter")
    {
      magFilter  = getXMLMagFilter(child->Attribute("mag"));
      minFilter  = getXMLMinFilter(child->Attribute("min"));
      aniso      = XMLArbiter::fillComponents1i(child, "aniso", 0);
      continue;
    }
  }

  result = (target == GL_TEXTURE_2D      ) ? load2D  (path, clampS, clampT, magFilter, minFilter, mipmap) :
           (target == GL_TEXTURE_CUBE_MAP) ? loadCube(path, clampS, clampT, magFilter, minFilter, mipmap) :
           Logger::writeErrorLog("Unsupported texture type");
  return result;
}

int Texture::getXMLMagFilter(const String &value)
{
  if(value.getLength())
  {
    if(value == "NEAREST") return GL_NEAREST;
  }
  return GL_LINEAR;
}

int Texture::getXMLMinFilter(const String &value)
{
  if(value.getLength())
  {
    if(value == "NEAREST_MIPMAP_NEAREST") return GL_NEAREST_MIPMAP_NEAREST;
    if(value == "LINEAR_MIPMAP_NEAREST")  return GL_LINEAR_MIPMAP_NEAREST;
    if(value == "NEAREST_MIPMAP_LINEAR")  return GL_NEAREST_MIPMAP_LINEAR;
    if(value == "LINEAR_MIPMAP_LINEAR")   return GL_LINEAR_MIPMAP_LINEAR;
    if(value == "NEAREST")                return GL_NEAREST;
  }
  return GL_LINEAR;
}

int Texture::getXMLWrapMode(const String &value)
{
  if(value.getLength())
  {
    if(value == "CLAMP_TO_BORDER") return GL_CLAMP_TO_BORDER;
    if(value == "CLAMP_TO_EDGE")   return GL_CLAMP_TO_EDGE;
    if(value == "REPEAT")          return GL_REPEAT;
  }
  return GL_CLAMP;
}

int Texture::getXMLType(const String &value)
{
  if(value.getLength())
  {
    if(value == "TEXTURE_1D")       return GL_TEXTURE_1D;
    if(value == "TEXTURE_3D")       return GL_TEXTURE_3D;
    if(value == "TEXTURE_CUBE_MAP") return GL_TEXTURE_CUBE_MAP;
  }
  return GL_TEXTURE_2D;
}

int Texture::getValidWrapMode(int clamp)
{
  return (clamp == GL_CLAMP          ) ? GL_CLAMP           :
         (clamp == GL_REPEAT         ) ? GL_REPEAT          :
         (clamp == GL_CLAMP_TO_EDGE  ) ? GL_CLAMP_TO_EDGE   :
         (clamp == GL_CLAMP_TO_BORDER) ? GL_CLAMP_TO_BORDER : GL_REPEAT;
}

int Texture::getValidMagFilter(int filter)
{
  return (filter == GL_NEAREST ) ? GL_NEAREST  : GL_LINEAR;
}

int Texture::getValidMinFilter(int filter)
{
  return (filter == GL_NEAREST                ) ? GL_NEAREST                :
         (filter == GL_LINEAR_MIPMAP_LINEAR   ) ? GL_LINEAR_MIPMAP_LINEAR   :
         (filter == GL_NEAREST_MIPMAP_LINEAR  ) ? GL_NEAREST_MIPMAP_LINEAR  :
         (filter == GL_LINEAR_MIPMAP_NEAREST  ) ? GL_LINEAR_MIPMAP_NEAREST  :
         (filter == GL_NEAREST_MIPMAP_NEAREST ) ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR;
}

bool Texture::exportXMLSettings(ofstream &xmlFile  )
{
  if(!xmlFile.is_open())
    return Logger::writeErrorLog("Cannot export Texture to XML: file not ready.");

  if(!id)
    return Logger::writeErrorLog("Cannot export Texture to XML: invalid texture handle.");

  TextureInfo *textureInfo  = TexturesManager::getTextureInfo(id);

  if(!textureInfo)
    return Logger::writeErrorLog("Cannot export Texture to XML: cannot locate a valid MediaInfo.");

  xmlFile << "  <Texture  type        = \"" 
          << ((target == GL_TEXTURE_2D      ) ? "TEXTURE_2D\"\n" :
              (target == GL_TEXTURE_1D      ) ? "TEXTURE_1D\"\n" :
              (target == GL_TEXTURE_3D      ) ? "TEXTURE_3D\"\n" : "TEXTURE_CUBE_MAP\"\n")
          << "            path        = \"" << textureInfo->getMediaPath() << "\"\n"
          << "            mipmap      = \"true\" > \n"
          << "    <Wrap   s           = \"REPEAT\" \n"
          << ((target == GL_TEXTURE_3D)  ? "          r           = \"REPEAT\" \n" : "")
          << "            t           = \"REPEAT\" />\n"
          << "    <Filter mag         = \"LINEAR\" \n"
          << "            min         = \"LINEAR_MIPMAP_LINEAR\"/> \n";

  xmlFile << "  </Texture>\n";
  return true;
}

void Texture::destroy()
{
  TextureInfo *textureInfo = TexturesManager::getTextureInfo(id);

  if(textureInfo)
    textureInfo->decreaseUserCount();
  else
    glDeleteTextures(1, &id);
 
  TexturesManager::flushUnusedTextures();

  height         = 0;
  width          = 0;
  depth          = 0;
  id             = 0;
}

Texture::~Texture()
{
  destroy();
}

bool Texture::load2D(const char* filename,
                     GLuint clampS   , GLuint clampT,
                     GLuint magFilter, GLuint minFilter,
                     bool  mipmap)
{
  const char  *texturePath = MediaPathManager::lookUpMediaPath(filename);

  if(!texturePath)
    return Logger::writeErrorLog(String("Couldn't locate the Texture file at <") +
                                 filename + "> even with a look up");

  if(checkForRepeat(texturePath))
    return true;

  Image  image;

  if(!image.load(filename))
    return Logger::writeErrorLog(String("Could not load Texture2D file at -> ") + filename);
  
  return load2DImage(image, clampS, clampT, magFilter, minFilter, mipmap);
}

bool Texture::load2DImage(const Image& image,
                          GLuint clampS   , GLuint clampT,
                          GLuint magFilter, GLuint minFilter,
                          bool  mipmap)
{
  const char *path = image.getPath();
  if(checkForRepeat(path))
    return true;

  destroy();
  target = GL_TEXTURE_2D;

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getValidMagFilter(magFilter));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getValidMinFilter(minFilter));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     getValidWrapMode(clampS));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     getValidWrapMode(clampT));
  
  if(loadTextureFace(image, GL_TEXTURE_2D, mipmap));
  else
  {
    destroy();
    return Logger::writeErrorLog(String("Could not load Texture2D file."));
  }
  
  height = image.getHeight();
  width  = image.getWidth();
  depth  = 1;
 
  if(finalizeLoading(path))
  {
    Logger::writeInfoLog(String("Loaded Texture2D file at -> ") + path);
    return true;
  }
  return false;
}

bool Texture::loadCube(const char* pathPXdotExtension,
                       GLuint clampS   , GLuint clampT,
                       GLuint magFilter, GLuint minFilter,
                       bool  mipmap)
{
  String initialPath  = MediaPathManager::lookUpMediaPath(pathPXdotExtension);
  String temp;

  if(!initialPath.getLength())
    return Logger::writeErrorLog(String("Couldn't locate the texture file at <") +
                                 pathPXdotExtension + "> even with a look up");
  destroy();
  target = GL_TEXTURE_CUBE_MAP;

  std::string  verifiedPaths[6],
               faces[6]     = { "PX", "NX", "PY", "NY", "PZ", "NZ"},
               body;

  int extensionIndex = 0;

  for(int j = 0; j < int(initialPath.getLength()); j++)
    if(initialPath[j] == '.')
      extensionIndex = j;

  if(extensionIndex == 0)
    return false;

  const  char   *extension  = initialPath.getBytes() + extensionIndex;
  GLint  strl        = 0;

  strl  = initialPath.getLength();
  strl -= int(strlen(extension)) + 2;

  for(int i = 0; i< 6; i++)
  {
    for(int l = 0; l < strl; l++)
      verifiedPaths[i] += initialPath[l];

    verifiedPaths[i] += faces[i];
    verifiedPaths[i] += extension;
    temp = MediaPathManager::lookUpMediaPath(verifiedPaths[i].data());

    if(temp.getLength())
       verifiedPaths[i] = temp;
    else
      return Logger::writeErrorLog(String("Failed to locate cubemap texture face ") + verifiedPaths[i].data());
  }

  if(checkForRepeat(verifiedPaths[0].data()))
    return true;

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, id);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, getValidMagFilter(magFilter));
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, getValidMinFilter(minFilter));
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S,     getValidWrapMode(clampS));
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T,     getValidWrapMode(clampT));

  for(int i = 0; i < 6; i++)
  {
    Image image;

    if(!image.load(verifiedPaths[i].data()))
    {
      destroy();
      return Logger::writeErrorLog(String("Failed at loading CubeMap face: ") + i);
    }

    if(!loadTextureFace(image, GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + i, mipmap))
      return Logger::writeErrorLog(String("Failed while loading CubeMap face: ") + i);

    if(i == 5)
    {
      height = image.getHeight();
      width  = image.getWidth();
      depth  = 1;
    }
  }

  Logger::writeInfoLog(String("Loaded TextureCubeMap at ->") + initialPath);
  return  finalizeLoading(initialPath);
}

bool Texture::create2DShell(const char* name,
                            GLuint width    , GLuint height, 
                            GLuint inFormat , GLuint format,
                            GLuint clampS   , GLuint clampT, 
                            GLuint magFilter, GLuint minFilter)
{
  if(!name)
    return Logger::writeErrorLog("2D Texture Shell must have a valid name");

  if(checkForRepeat(name))
    return true;

  destroy();
  target = GL_TEXTURE_2D;

  Logger::writeInfoLog(String("Loading new 2D Shell: width = ") + int(width)
                       + ", height = " + int(height));

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getValidMagFilter(magFilter));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getValidMinFilter(minFilter));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     getValidWrapMode(clampS));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     getValidWrapMode(clampT));
  glTexImage2D(GL_TEXTURE_2D, 0, inFormat, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
 
  this->height = height;
  this->width  = width;
  depth        = 1;

  return finalizeLoading(name);
}

bool Texture::createNoise3D(const char* name, GLuint size)
{
  if(!name)
    return Logger::writeErrorLog("Noise3D Texture must have a valid name");

  if(checkForRepeat(name))
    return true;

  destroy();
  target = GL_TEXTURE_3D;

  Logger::writeInfoLog(String("Creating new Noise3D Texture: width = ") 
                       + int(size) + ", height = " + int(size) + ", depth =" + int(size));


  GLubyte *noise3DBuffer = new GLubyte[size * size * size * 4],
          *ptr           = NULL;
 
  Tuple3d  ni;

  double   inci= 0.0, 
           incj = 0.0,
           inck = 0.0,
           amp  = 0.5;

  GLuint   f, i, j, k, inc,
           startFrequency = 4,
           frequency      = 4,
           numOctaves     = 4;

  for (f = 0, inc = 0; f < numOctaves; ++f, frequency *= 2, ++inc, amp *= 0.5)
  {
    Perlin::setNoiseFrequency(frequency);
  	ptr = noise3DBuffer;
   	ni.set(0.0, 0.0, 0.0);

		inci = 1.0 / (size / frequency);
	  for (i = 0; i < size; ++i, ni[0] += inci)
	  {
      incj = 1.0 / (size / frequency);
      for (j = 0; j < size; ++j, ni[1] += incj)
      {
		  	inck = 1.0 / (size / frequency);
		  	for (k = 0; k < size; ++k, ni[2] += inck, ptr += 4)
          *(ptr + inc) = (GLubyte) (((Perlin::noise3(ni) + 1.0) * amp) * 128.0);
		  }
    }
	}
 
  glGenTextures(1, &id);
  glBindTexture(target, id);
  glTexParameterf(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(target, GL_TEXTURE_WRAP_R, GL_REPEAT);
  glTexParameterf(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage3DEXT(target, 0, GL_RGBA,
                  size, size, size, 
                  0, GL_RGBA, GL_UNSIGNED_BYTE, noise3DBuffer);
  deleteArray(noise3DBuffer);
  return true;
}

bool Texture::createRectShell(const char* name,
                              GLuint width, GLuint height,
                              GLuint inFormat, GLuint format,  
                              GLuint clampS  , GLuint clampT, 
                              GLuint magFilter, GLuint minFilter)
{
 if(!name)
    return Logger::writeErrorLog("Rectangle Texture Shell must have a valid name");

  if(checkForRepeat(name))
    return true;

  destroy();
  target = GL_TEXTURE_RECTANGLE_ARB;

  Logger::writeInfoLog(String("Loading new Rectangle Shell: width = ") + int(width)
                       + ", height = " + int(height));

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, id);

  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, getValidMagFilter(magFilter));
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, getValidMinFilter(minFilter));
  glTexParameteri(target, GL_TEXTURE_WRAP_S,     getValidWrapMode(clampS));
  glTexParameteri(target, GL_TEXTURE_WRAP_T,     getValidWrapMode(clampT));
  glTexImage2D(target, 0, inFormat, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
 
  this->height = height;
  this->width  = width;
  depth        = 1;

  return finalizeLoading(name);
}

bool Texture::create1DShell(const char* name,
                            GLuint width    ,
                            GLuint inFormat , GLuint format,
                            GLuint clampS   , GLuint clampT, 
                            GLuint magFilter, GLuint minFilter)
{
  if(!name)
    return Logger::writeErrorLog("1D Texture Shell must have a valid name");
  destroy();
  target = GL_TEXTURE_1D;

  
  Logger::writeInfoLog(String("Loading new 1D Shell: width = ") + int(width)
                       + ", height = 1");

  glGenTextures(1, &id);
  glBindTexture(target, id);

  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, getValidMagFilter(magFilter));
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, getValidMinFilter(minFilter));
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S,     getValidWrapMode(clampS));
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T,     getValidWrapMode(clampT));
  glTexImage1D   (GL_TEXTURE_1D, 0, inFormat, width, 0, format, GL_UNSIGNED_BYTE, NULL);

  this->width  = width;
  height       = 1;
  depth        = 1;

  return finalizeLoading(name);
}

bool Texture::create3DShell(const char* name,
                            GLuint width, GLuint height, GLuint depth, 
                            GLuint inFormat , GLuint format,
                            GLuint clampS   , GLuint clampT, GLuint clampR,  
                            GLuint magFilter, GLuint minFilter)
{
  if(!name)
    return Logger::writeErrorLog("3D Texture Shell must have a valid name");
  destroy();
  target = GL_TEXTURE_3D;

  
  Logger::writeInfoLog(String("Loading new 3D Shell: width = ") + int(width)
                       + ", height = " + int(height) + ", depth = "  + int(depth));

  glGenTextures(1, &id);
  glBindTexture(target, id);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, getValidMagFilter(magFilter));
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, getValidMinFilter(minFilter));
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S,     getValidWrapMode(clampS));
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T,     getValidWrapMode(clampT));
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R,     getValidWrapMode(clampR));
  glTexImage3D   (GL_TEXTURE_3D, 0, inFormat, width, height, depth, 0, format, GL_UNSIGNED_BYTE, NULL);

  this->height = height;
  this->width  = width;
  this->depth  = depth;

  return finalizeLoading(name);
}

bool Texture::createCubeShell(const char* name,
                              GLuint size     ,
                              GLuint inFormat , GLuint format,
                              GLuint clampS   , GLuint clampT, 
                              GLuint magFilter, GLuint minFilter)
{
  if(!name)
    return Logger::writeErrorLog("Cube Texture Shell must have a valid name");

  if(checkForRepeat(name))
    return true;

  destroy();
  target = GL_TEXTURE_CUBE_MAP;

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, id);

  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, getValidMagFilter(magFilter));
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, getValidMinFilter(minFilter));
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S,     getValidWrapMode(clampS));
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T,     getValidWrapMode(clampT));

  Logger::writeInfoLog(String("Loading Cube Shell: size = ") + int(size));

  for(int i = 0; i < 6; i++)
   glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + i,
                0, 
                inFormat,
                size, 
                size,
                0,
                format, 
                GL_UNSIGNED_BYTE, 
                NULL);

  this->height = size;
  this->width  = size;
  depth        = 1;

  return finalizeLoading(name);
}

bool Texture::createNormalizingCube(const char* name,
                                    GLuint size, 
                                    bool   buildMipmaps) 
{
  size = getClosestPowerOfTwo(size);

  if(!name)
    return Logger::writeErrorLog("Normalized Cube Texture must have a valid name");

  if(checkForRepeat(name))
    return true;

  destroy();

  int levels = buildMipmaps ? int(log(double(size))/log(2.0)) : 1;
  target = GL_TEXTURE_CUBE_MAP;
  Logger::writeInfoLog(String("Creating Normalization Texture Cube Map, size = ")
                       + int(size) + ", MipMaps = " + int(levels));
 
  GLuint minFilter = buildMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, id);

  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, minFilter);


  for(int currentLevel = 0; currentLevel < levels; currentLevel++)
  {
    float fX    = 0.0,
          fY    = 0.0,
			    fZ    = 0.0,
          oolen = 1.0;
	  int x            =    0,
        y            =    0,
        z            =    0,
        mapSize      = size/(currentLevel + 1), 
        halfMapSize  = mapSize/2;

  	GLubyte *dataContainer = new GLubyte[mapSize*mapSize*3];

	  for(y=0;y<mapSize;y++)
	  {
		  for(z=0;z<mapSize;z++)
	  	{
		    fX    = (float)halfMapSize;
        fY    = (float)halfMapSize-y;
        fZ    = (float)halfMapSize-z;
		    oolen = 1.0f/fastSquareRoot(fX*fX+fY*fY+fZ*fZ);

			  fX*=oolen;
			  fY*=oolen;
			  fZ*=oolen;

		  	dataContainer[y*3*mapSize+z*3+0] = GLubyte((((fX)+1.0f)/2.0f)*255.0f);
		  	dataContainer[y*3*mapSize+z*3+1] = GLubyte((((fY)+1.0f)/2.0f)*255.0f);
			  dataContainer[y*3*mapSize+z*3+2] = GLubyte((((fZ)+1.0f)/2.0f)*255.0f);
	  	}
	  }  

	  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + 0,
                 currentLevel, GL_RGB8, mapSize, mapSize,
                 0, GL_RGB, GL_UNSIGNED_BYTE, dataContainer);

    for(y=0;y<mapSize;y++)
		{
			for(z=0;z<mapSize;z++)
			{
				fX    =(float)-halfMapSize;
				fY    =(float)halfMapSize-y;
				fZ    =(float)z-halfMapSize;
				oolen = 1.0f/fastSquareRoot(fX*fX+fY*fY+fZ*fZ);

				fX*=oolen;
				fY*=oolen;
				fZ*=oolen;
				dataContainer[y*3*mapSize+z*3+0] = GLubyte((((fX)+1.0f)/2.0f)*255.0f);
				dataContainer[y*3*mapSize+z*3+1] = GLubyte((((fY)+1.0f)/2.0f)*255.0f);
				dataContainer[y*3*mapSize+z*3+2] = GLubyte((((fZ)+1.0f)/2.0f)*255.0f);
			} 
		}

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + 1,
                 currentLevel, GL_RGB8, mapSize, mapSize,
                 0, GL_RGB, GL_UNSIGNED_BYTE, dataContainer);

		for(z=0;z<mapSize;z++)
		{
			for(x=0;x<mapSize;x++)
			{
				fX    = (float)x-halfMapSize;
				fY    = (float)halfMapSize;
				fZ    = (float)z-halfMapSize;
				oolen = 1.0f/fastSquareRoot(fX*fX+fY*fY+fZ*fZ);

				fX*=oolen;
				fY*=oolen;
				fZ*=oolen;
				dataContainer[z*3*mapSize+x*3+0] = GLubyte((((fX)+1.0f)/2.0f)*255.0f);
				dataContainer[z*3*mapSize+x*3+1] = GLubyte((((fY)+1.0f)/2.0f)*255.0f);
				dataContainer[z*3*mapSize+x*3+2] = GLubyte((((fZ)+1.0f)/2.0f)*255.0f);
			}
		}

	  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + 2,
                 currentLevel, GL_RGB8, mapSize, mapSize,
                 0, GL_RGB, GL_UNSIGNED_BYTE, dataContainer);

		for(z=0;z<mapSize;z++)
		{
			for(x=0;x<mapSize;x++)
			{
				fX    = (float)x-halfMapSize;
				fY    = (float)-halfMapSize;
				fZ    = (float)halfMapSize-z;
				oolen = 1.0f/fastSquareRoot(fX*fX+fY*fY+fZ*fZ);

				fX*=oolen;
				fY*=oolen;
				fZ*=oolen;

				dataContainer[z*3*mapSize+x*3+0] = GLubyte((((fX)+1.0f)/2.0f)*255.0f);
			  dataContainer[z*3*mapSize+x*3+1] = GLubyte((((fY)+1.0f)/2.0f)*255.0f);
				dataContainer[z*3*mapSize+x*3+2] = GLubyte((((fZ)+1.0f)/2.0f)*255.0f);
			}
		}

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + 3,
                 currentLevel, GL_RGB8, mapSize, mapSize,
                 0, GL_RGB, GL_UNSIGNED_BYTE, dataContainer);

		for(y=0;y<mapSize;y++)
		{
			for(x=0;x<mapSize;x++)
			{
				fX    = (float)x-halfMapSize;
				fY    = (float)halfMapSize - y;
				fZ    = (float)halfMapSize;
				oolen = 1.0f/fastSquareRoot(fX*fX+fY*fY+fZ*fZ);

				fX*=oolen;
				fY*=oolen;
				fZ*=oolen;

				dataContainer[y*3*mapSize+x*3+0] = GLubyte((((fX)+1.0f)/2.0f)*255.0f);
	      dataContainer[y*3*mapSize+x*3+1] = GLubyte((((fY)+1.0f)/2.0f)*255.0f);
				dataContainer[y*3*mapSize+x*3+2] = GLubyte((((fZ)+1.0f)/2.0f)*255.0f);
			}
		}

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + 4,
                 currentLevel, GL_RGB8, mapSize, mapSize,
                 0, GL_RGB, GL_UNSIGNED_BYTE, dataContainer);

		for(y=0;y<mapSize;y++)
		{
			for(x=0;x<mapSize;x++)
			{
				fX    = (float)halfMapSize - x;
				fY    = (float)halfMapSize - y;
				fZ    = (float)-halfMapSize;
				oolen = 1.0f/fastSquareRoot(fX*fX+fY*fY+fZ*fZ);

				fX*=oolen;
				fY*=oolen;
				fZ*=oolen;

				dataContainer[y*3*mapSize+x*3+0] = GLubyte((((fX)+1.0f)/2.0f)*255.0f);
				dataContainer[y*3*mapSize+x*3+1] = GLubyte((((fY)+1.0f)/2.0f)*255.0f);
				dataContainer[y*3*mapSize+x*3+2] = GLubyte((((fZ)+1.0f)/2.0f)*255.0f);
			}
		}
	  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + 5,
                 currentLevel, GL_RGB8, mapSize, mapSize,
                 0, GL_RGB, GL_UNSIGNED_BYTE, dataContainer);

	  deleteArray(dataContainer);
  }
  this->height = size;
  this->width  = size;
  depth        = 1;
  return finalizeLoading(name);
}

bool Texture::loadTextureFace(const  Image &image,
                              GLuint target, 
                              bool   mipmap)
{
  if(!id)
    return Logger::writeErrorLog("Invalid texture id, Texture::loadTextureFace");

  if((this->target != GL_TEXTURE_2D) &&  (this->target != GL_TEXTURE_CUBE_MAP))
    return Logger::writeErrorLog("Unrecognized texture target, Texture::loadTextureFace");

  GLuint quickFix = 0;
  switch(target)
  {
    case GL_TEXTURE_2D: 
      quickFix = GL_TEXTURE_2D;
    break;

    case GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB: 
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB:
      quickFix = GL_TEXTURE_CUBE_MAP_ARB;
     break;
  }
  
  switch(image.getInternalFormat()) 
  {
    case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
      if(GLEE_ARB_texture_compression &&
         GLEE_EXT_texture_compression_s3tc)
      {

        if(GLEE_SGIS_generate_mipmap && mipmap)
        {
          glTexParameteri(quickFix, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
          glTexParameteri(quickFix, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        }

        glCompressedTexImage2DARB(target,
                                  0,
                                  GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
                                  image.getWidth(), image.getHeight(), 
                                  0, 
                                  ((image.getWidth() +3)/4)*((image.getHeight() +3)/4)*16, 
                                  image.getDataBuffer());
      }
      else
      {
        destroy();
        return Logger::writeErrorLog("GL_EXT_texture_compression_s3tc not supported");
      } 
    break;

    case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
      if(GLEE_ARB_texture_compression &&
         GLEE_EXT_texture_compression_s3tc)
      {

        if(GLEE_SGIS_generate_mipmap && mipmap)
        {
          glTexParameteri(quickFix, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
          glTexParameteri(quickFix, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        }

        glCompressedTexImage2DARB(target,
                                  0,
                                  GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 
                                  image.getWidth(), image.getHeight(), 
                                  0, 
                                  ((image.getWidth() +3)/4)*((image.getHeight() +3)/4)*16,
                                  image.getDataBuffer());
      }
      else
      {
        destroy();
        return Logger::writeErrorLog("GL_EXT_texture_compression_s3tc not supported");
      } 
    break;

    case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT: 
      if(GLEE_ARB_texture_compression &&
         GLEE_EXT_texture_compression_s3tc)
      {
        if(GLEE_SGIS_generate_mipmap && mipmap)
        {
          glTexParameteri(quickFix, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
          glTexParameteri(quickFix, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        }

        glCompressedTexImage2DARB(target, 
                                  0,
                                  GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 
                                  image.getWidth(), image.getHeight(), 
                                  0,
                                  ((image.getWidth() +3)/4)*((image.getHeight() +3)/4)*8, 
                                  image.getDataBuffer());
      }
      else
      {
        destroy();
        return Logger::writeErrorLog("GL_EXT_texture_compression_s3tc not supported");
      } 
    break;

    default: 
      if(mipmap)
        gluBuild2DMipmaps(target,
                          image.getInternalFormat(),
                          image.getWidth(),
                          image.getHeight(),
                          image.getFormat(),
                          GL_UNSIGNED_BYTE,
                          image.getDataBuffer());
     else
       glTexImage2D(target, 0,  image.getInternalFormat(), image.getWidth(), 
                    image.getHeight(), 0, image.getFormat(), 
                    GL_UNSIGNED_BYTE, image.getDataBuffer());

  }
  return true;
}

/*******************************************************************************************/
/*TexturesManager                                                                          */
/*Info:                                                                                    */
/*                                                                                         */
/*******************************************************************************************/


vector<TextureInfo*> TexturesManager::textureCollection;

bool TexturesManager::addTextureInfo(TextureInfo *textureInfo)
{
  flushUnusedTextures();

  if(!textureInfo)
    return false;
  textureCollection.push_back(textureInfo);
  return true;
}

TextureInfo *TexturesManager::getTextureInfo(const char* texturePath)
{
  flushUnusedTextures();
  if(!texturePath || !textureCollection.size())
    return NULL;

  for(size_t i = 0; i < textureCollection.size(); i++)
    if(textureCollection[i]->getMediaPath() == texturePath)
      return textureCollection[i];

  return NULL;
}

TextureInfo *TexturesManager::getTextureInfo(GLuint textureID)
{
  if(!textureID || !textureCollection.size())
    return NULL;

  for(size_t i = 0; i < textureCollection.size(); i++)
    if(textureCollection[i]->getMedia() == textureID)
      return textureCollection[i];

  return NULL;
}

void TexturesManager::flushUnusedTextures()
{
  vector<TextureInfo*> validTextures;
  size_t i         = 0,
         count     = 0;
  GLuint textureID = 0;

  for(i = 0; i < textureCollection.size(); i++)
    if(textureCollection[i]->getUserCount() > 0)
    {
      validTextures.push_back(textureCollection[i]);
    }
    else
    {
      count++;
      textureID = textureCollection[i]->getMedia();
      glDeleteTextures(1, &textureID);
      deleteObject(textureCollection[i]);
    }

  textureCollection.clear();

  for(i = 0; i < validTextures.size(); i++)
      textureCollection.push_back(validTextures[i]);

  if(count)
    Logger::writeInfoLog(String("Flushed textures count -> ") + int(count));
}

void TexturesManager::flushAllTextures(){
  GLuint textureID = 0;

  size_t i = 0;
  for(i = 0; i < textureCollection.size(); i++)
  {
    textureID = textureCollection[i]->getMedia();
    glDeleteTextures(1, &textureID);
    deleteObject(textureCollection[i]);
  }

  textureCollection.clear();
  if(i) Logger::writeInfoLog("Flushed all active textures");
}

void TexturesManager::printTexturesInfo()
{
  if(!textureCollection.size())
    cout << "This Manager contains no Textures as of yet." << endl;
  else{
    cout << "Textures list: " << endl;
    for(size_t i = 0; i < textureCollection.size(); i++)
      cout <<  "<users = \"" << textureCollection[i]->getUserCount() << "\" "
           <<  " path  = \"" << textureCollection[i]->getMediaPath() << "\" "
           <<  " id    = \"" << textureCollection[i]->getMedia()     << "\" />" << endl;
  }
}
