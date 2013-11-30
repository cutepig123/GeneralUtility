#ifndef GUI_EVENT_H
#define GUI_EVENT_H

#include "../Tools/NamedObject.h"

class GUIRectangle;
class GUIEvent
{
  public:
    GUIEvent(GUIRectangle *element);
    GUIEvent(const GUIEvent& copy);
    GUIEvent &operator= (const GUIEvent& copy);

    GUIRectangle *getEventSource();
    const String &getCallbackString();

  private:
    GUIRectangle *eventSource;
    String        callbackString;
};

#endif