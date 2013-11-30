#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

#include "GUIAlphaElement.h"
#include "GUIClippedRectangle.h"

class GUIButton : public GUIAlphaElement, public GUIClippedRectangle
{
  public:
    GUIButton(const char *callbackString = NULL);

    virtual void  render(float clockTick);
    virtual bool  loadXMLSettings(const TiXmlElement *node);
    virtual const void computeWindowBounds();

    void enableBounce(bool bounce);
    bool bounceEnabled();

  private:
    bool bounce;
};

#endif
