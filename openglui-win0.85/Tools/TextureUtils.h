#ifndef TEXTURE_H
#define TEXTURE_H

#include "Image.h"
#include "../Tools/MediaInfo.h"

typedef MediaInfo<unsigned int> TextureInfo;

class TexturesManager
{
  public:
    static bool        addTextureInfo(TextureInfo *textureInfo);
    static TextureInfo *getTextureInfo(const char* texturePath);
    static TextureInfo *getTextureInfo(GLuint textureID);
    static void        flushUnusedTextures();

    static void printTexturesInfo();
    static void flushAllTextures();
  private:
    static vector<TextureInfo*> textureCollection;

};

class Texture : public IOXMLObject
{
  public:
    Texture(GLuint target = GL_TEXTURE_2D);
    Texture(const Texture &copy);
    Texture &operator = (const Texture &);
   ~Texture();

    virtual bool loadXMLSettings(const TiXmlElement *element);
    virtual bool exportXMLSettings(ofstream &xmlFile  );

    bool load2D(const char* infoEndOrPath,
                GLuint clampS    = GL_REPEAT,
                GLuint clampT    = GL_REPEAT,
                GLuint magFilter = GL_LINEAR,
                GLuint minFilter = GL_LINEAR_MIPMAP_LINEAR,
                bool   mipmap    = true);

    bool load2DImage(const Image &image,
                     GLuint clampS    = GL_REPEAT,
                     GLuint clampT    = GL_REPEAT,
                     GLuint magFilter = GL_LINEAR,
                     GLuint minFilter = GL_LINEAR_MIPMAP_LINEAR,
                     bool  mipmap    = true);

    bool loadCube(const char* infoEndOrPath,
                  GLuint clampS    = GL_CLAMP_TO_EDGE,
                  GLuint clampT    = GL_CLAMP_TO_EDGE,
                  GLuint magFilter = GL_LINEAR,
                  GLuint minFilter = GL_LINEAR_MIPMAP_LINEAR,
                  bool  mipmap    = true);

    bool create1DShell(const char* name,
                       GLuint width     = 128,
                       GLuint inFormat  = GL_RGBA8,
                       GLuint format    = GL_RGBA, 
                       GLuint clampS    = GL_REPEAT, 
                       GLuint clampT    = GL_REPEAT, 
                       GLuint magFilter = GL_LINEAR,
                       GLuint minFilter = GL_LINEAR);

    bool create2DShell(const char* name,
                       GLuint width     = 128,
                       GLuint height    = 128, 
                       GLuint inFormat  = GL_RGBA8,
                       GLuint format    = GL_RGBA, 
                       GLuint clampS    = GL_REPEAT, 
                       GLuint clampT    = GL_REPEAT, 
                       GLuint magFilter = GL_LINEAR,
                       GLuint minFilter = GL_LINEAR);

    bool createRectShell(const char* name,
                         GLuint width     = 128,
                         GLuint height    = 128, 
                         GLuint inFormat  = GL_RGBA8,
                         GLuint format    = GL_RGBA,  
                         GLuint clampS    = GL_CLAMP_TO_EDGE, 
                         GLuint clampT    = GL_CLAMP_TO_EDGE, 
                         GLuint magFilter = GL_LINEAR,
                         GLuint minFilter = GL_LINEAR);

    bool create3DShell(const char* name,
                       GLuint width     = 64,
                       GLuint height    = 64, 
                       GLuint depth     = 64,
                       GLuint inFormat  = GL_RGBA8,
                       GLuint format    = GL_RGBA, 
                       GLuint clampS    = GL_REPEAT, 
                       GLuint clampT    = GL_REPEAT, 
                       GLuint clampR    = GL_REPEAT, 
                       GLuint magFilter = GL_LINEAR,
                       GLuint minFilter = GL_LINEAR);

    bool createCubeShell(const char* name,
                         GLuint size      = 128,
                         GLuint inFormat  = GL_RGBA8,
                         GLuint format    = GL_RGBA, 
                         GLuint clampS    = GL_CLAMP_TO_EDGE, 
                         GLuint clampT    = GL_CLAMP_TO_EDGE, 
                         GLuint magFilter = GL_LINEAR,
                         GLuint minFilter = GL_LINEAR);

    bool createNoise3D(const  char* name,
                       GLuint size = 128);

    bool createNormalizingCube(const char* name,
                               GLuint size         = 128, 
                               bool   buildMipmaps = true);

    void copyCurrentBuffer(int newTarget = -1);

    const bool activate(int unit = -1);
    const bool deactivate();

 
    void   setID(GLuint textureID);
    const  GLuint  getID() const;

    void   setTarget(GLuint target);
    const GLuint  getTarget() const;

    const GLuint  getHeight() const;
    const GLuint  getWidth()  const;
    const GLuint  getDepth()  const;
    void    destroy();

  protected:
    GLuint                 height,
                           width,
                           depth;
  private:  
    GLuint                 target,
                           id;
    int                    currUnit;

    bool loadTextureFace(const Image &image, GLuint target, bool   mipmap);

    bool finalizeLoading(const char* string);
    bool checkForRepeat (const char* string);
 
    int getXMLMagFilter(const String &value);
    int getXMLMinFilter(const String &value);
    int getXMLWrapMode (const String &value);
    int getXMLType     (const String &value);

    int getValidWrapMode (int clamp);
    int getValidMagFilter(int filter);
    int getValidMinFilter(int filter);

};
#endif
