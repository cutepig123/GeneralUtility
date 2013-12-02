#ifndef GUI_ELEMENT_H
#define GUI_ELEMENT_H

#include "GUIRectangle.h"
#include "GUIText.h"

class GUIAlphaElement : public GUIRectangle
{

  protected:
    GUIText    label;
    Tuple4f    color;
    float      alphaFadeScale,
               minAlpha;

    void       modifyCurrentAlpha(float tick);
 public:
    GUIAlphaElement(const char *callback = NULL);

    virtual void render(float clockTick) = 0;
    virtual bool loadXMLSettings(const TiXmlElement *node);

    void  setMinAlpha(float minAlpha);
    float getMinAlpha();

    void  setAlphaFadeScale(float scale);
    float getAlphaFadeScale();

    void  setAlpha(float alpha);
    float getAlpha();

    void  setLabelString(const char *label);
    void  setLabelString(const String &label);

    const char*   getLabelString();
    void  setColor(const Tuple3f& color);
    void  setColor(float x, float y, float z);
    const Tuple4f &getColor();

    GUIText * getLabel();
};

#endif
