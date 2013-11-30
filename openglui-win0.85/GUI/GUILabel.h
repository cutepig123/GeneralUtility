#ifndef GUI_LABEL_H
#define GUI_LABEL_H

#include "GUIAlphaElement.h"

class GUILabel : public GUIAlphaElement
{
  public:
    GUILabel(const char   *labelString    = NULL,
             const char   *callbackString = NULL);

    virtual void  render(float clockTick);
    virtual bool  loadXMLSettings(const TiXmlElement *element);
    virtual const Tuple4i &getWindowBounds();
};

#endif
