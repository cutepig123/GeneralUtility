#ifndef GUI_INPUTEVENTLISTENER_H
#define GUI_INPUTEVENTLISTENER_H

#include "GUIEvent.h"


class GUIEventListener
{
  public:
    virtual void actionPerformed(GUIEvent &evt) = 0;
};

#endif
