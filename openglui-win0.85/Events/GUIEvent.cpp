#include "GUIEvent.h"
#include "../GUI/GUIUtils.h"

GUIEvent::GUIEvent(GUIRectangle *element)
{
  callbackString = element? element->getCallbackString() : "Unknown";
  eventSource    = element;
  consumed       = false;
}

GUIEvent::GUIEvent(const GUIEvent& copy)
{
  this->operator =(copy);
}

GUIEvent &GUIEvent::operator= (const GUIEvent& copy)
{
  if(this != &copy)
  {
    callbackString = copy.callbackString;
    eventSource    = copy.eventSource;
    consumed       = copy.consumed;
  }
  return *this;
}

void GUIEvent::setConsumed(bool consumedArg)
{
  consumed = consumedArg;
}

bool GUIEvent::isConsumed()
{
  return consumed;
}
GUIRectangle *GUIEvent::getEventSource()   { return eventSource ;   }
const String &GUIEvent::getCallbackString(){ return callbackString; }
