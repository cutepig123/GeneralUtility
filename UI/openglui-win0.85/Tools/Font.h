/* Font
 *
 * Copyright (C) 2003-2004, Alexander Zaprjagaev <frustum@frustum.org>
 * Modified by Abdul Bezrati <abezrati@hotmail.com>
 */

#ifndef __FONT_H__
#define __FONT_H__

#include "TextureUtils.h"

class CFont
{
  public:
    CFont();
   ~CFont();

    void printProportional(float widthRatio , float heightRatio,
                           float width,       float height,
                           float r,  float g, float b,
                           const String &string);

    void print(float x,     float y,
               float width, float height,
               float r,     float g, float b,
               const String &string);

    void printSubString(float x,     float y,
                        float width, float height,
                        float r,     float g, float b,
                        int   start, int end,
                        const String &string);

    
    Tuple2i getStringDimensions(const String &string);
    bool    load(const char *fontPath);
    int     getMaxFittingLength(const String &string, int bounds);
    int     getHeight();
    const   int *getCharHorizontalGlyphs() const;

  private:
    Texture   fontTexture;
    int       spaces[256],
              fontHeight;
    GLint     fontBaseRange;
};

#endif /* __FONT_H__ */
