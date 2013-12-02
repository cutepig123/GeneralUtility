#ifndef GUI_CLIPPED_RECTANGLE_H
#define GUI_CLIPPED_RECTANGLE_H

#include "GUIRectangle.h"
#include "../Tools/TextureUtils.h"

class GUIClippedRectangle
{
  protected:
    Texture  texture;
    Tuple2f  texCoords[7];
 
    Tuple4f  textureRectangle,
             bgColor;

    Tuple3f  bordersColor;
    Tuple2i  vertices[7];
    bool     drawBackground,
             drawBounds;
    int      clipSize;

  public:
    GUIClippedRectangle(int clipSize = 5);
    void  enableBGColor(bool enable);
    bool  isBGColorOn();

    void  computeClippedBounds(const Tuple4i &windowBounds);

    void  setClipSize(int clipSize);
    int   getClipSize();

    void  setVisibleBounds(bool visible);
    bool  boundsVisible();

    void  setBGColor(float x, float y, float z, float alpha);
    void  setBGColor(const Tuple4f& color);
    const Tuple4f &getBGColor();
    const Tuple2i *getVertices() const;

    void  setBordersColor(const Tuple3f& color);
    void  setBordersColor(float x, float y, float z);
    const Tuple3f &getBordersColor();
    const Tuple2f *getTexCoords() const;

    void  setTextureRectangle(float x, float y, float z, float w);
    void  setTextureRectangle(const Tuple4f &textureRectangle);
    const Tuple4f &getTextureRectangle(); 

    Texture &getTexture(); 
    void     setTexture(const Texture & texture);

    virtual void renderClippedBounds();
    virtual bool loadXMLClippedRectangleInfo(const TiXmlElement *element);
};

#endif
