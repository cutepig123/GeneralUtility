#ifndef GUI_FONT_MANAGER_H
#define GUI_FONT_MANAGER_H

#include "GUIFont.h"

class GUIFontManager
{
  public:
    static int      addFont(const TiXmlElement *fontNodex);
    static int      findFontIndex(GUIFont *font);
    static GUIFont *getFont(size_t index);

    static void     setCurrentFont(GUIFont *currentFont);
    static void     setCurrentFont(size_t index);
    static GUIFont *getCurrentFont();
  
    static void     setDefaultFont(GUIFont *defaultFont);
    static void     setDefaultFont(size_t index);
    static GUIFont *getDefaultFont();
    static int      getCharacterWidth(char Char, GUIFont *font);
    static int      getCharacterWidth(char Char, int      index);

  private:
    static bool addFont(GUIFont *font);

    static vector<GUIFont *>  guiFontList;

    static GUIFont *currentFont,
                   *defaultFont;
};
#endif