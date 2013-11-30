#ifndef GUI_CBOX_H
#define GUI_CBOX_H

#include "GUIAlphaElement.h"

class GUICheckBox : public GUIAlphaElement
{
  public:
    GUICheckBox(const char  *callbackString = NULL);

    virtual void render(float clockTick);
    virtual void checkMouseEvents(MouseEvent &evt, int extraInfo, bool reservedBits = false);
    virtual bool loadXMLSettings(const TiXmlElement *node);
    virtual void setChecked(bool enabled);

    void  setAlphaMark(float mark);
    float getAlphaMark();

    void  setMinAlphaMark(float mark);
    float getMinAlphaMark();

    bool  isChecked();

    void  setBGColor(const Tuple3f& color);
    void  setBGColor(float x, float y, float z);
    const Tuple3f &getBGColor();
    virtual const Tuple4i &getWindowBounds();

  protected:
    Tuple3f bgColor;
    float   markRatio, 
            alphaMark,
            minAlphaMark;
    bool    checked;
    int     extra,
            markOffset,
            primaryTexDesc,
            secondaryTexDesc;
};

#endif
