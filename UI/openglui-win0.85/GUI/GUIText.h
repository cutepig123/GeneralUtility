#ifndef GUI_TEXT_H
#define GUI_TEXT_H

#include "../Tools/String.h"
#include "GUIRectangle.h"

class GUIFont;
class GUIText
{
  public:
    GUIText(const String &text);
    GUIText(const char *text = NULL);

    GUIText(const GUIText & text);
    GUIText &operator =(const GUIText& text);
    GUIText &operator =(const String & text);
    GUIText &operator =(const char*    text);

    virtual bool loadXMLSettings(const TiXmlElement *node);

    void  computeDimensions();
    void  printCenteredXY(int x, int y, int startIndex = 0, int endIndex = -1);
    void  printCenteredX (int x, int y, int startIndex = 0, int endIndex = -1);
    void  printCenteredY (int x, int y, int startIndex = 0, int endIndex = -1);
    void  print(int x, int y, int startIndex = 0, int endIndex = -1);

    const String& getString();
    const char*   getCharString();

    void setString(const String &text);
    void setString(const char   *text);
    void clear();

    void  setSize(int x, int y);
    void  setSize(const Tuple2i& size);
    const Tuple2i& getSize();

    void  setColor(float r, float g, float b);
    void  setColor(const Tuple3f &color);
    const Tuple3f &getColor();

    int   getHeight();
    int   getWidth();

    void   setHeightScale(float hs);
    void   setWidthScale(float ws);
    void   setScales(Tuple2f scales);
 
    float  getHeightScale();
    float  getWidthScale();  
    const  Tuple2f & getScales();

    bool  needUpdating();
    void  forceUpdate(bool update);

    void setFontIndex(int fontIndex);
    int  getFontIndex();

  protected:
    bool     update;
    String   text;
    Tuple2i  position,
             size;
    Tuple2f  scales;
    Tuple3f  color;
    int      fontIndex;
};

#endif
