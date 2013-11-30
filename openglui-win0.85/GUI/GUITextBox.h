#ifndef GUI_TEXT_BOX_H
#define GUI_TEXT_BOX_H

#include "GUIAlphaElement.h"

enum TextEditionType
{
  BACKSPACE_DELETE,
  SIMPLE_DELETE,
  INSERT_CHAR,
  BATCH_DELETE,
  PARSE_VISIBLE
};

class GUITextBox : public GUIAlphaElement
{
  public:
    GUITextBox(const char *callbackString = NULL,
               const char *fieldText      = NULL);

    virtual void    render(float clockTick);
    virtual void    checkMouseEvents(MouseEvent &evt, int extraInfo, bool reservedBits = false);
    virtual void    checkKeyboardEvents(KeyEvent evt, int extraInfo);
    virtual bool    loadXMLSettings(const TiXmlElement *node);
    virtual const   Tuple4i &getWindowBounds();
    void            setText(const String &text);
    const   String &getText() const;

    bool    textChanged();

    void  setBordersColor(const Tuple3f& color);
    void  setBordersColor(float r, float g, float b);
    const Tuple3f &getBordersColor() const;

    void  setPadding(const Tuple2i& p);
    void  setPadding(int x, int y);
    const Tuple2i &getPadding() const;

  protected:
    Tuple2i padding;
    Tuple3f bordersColor,
            textColor;
    float   blinkerTimer;
    bool    blinkerOn;
    int     blinkerPosition,
            textStartIndex,
            textEndIndex;
  private:
    void    setupText(int type, char info = NULL);
    void    setupBlinker(int mouseX);
};

#endif
