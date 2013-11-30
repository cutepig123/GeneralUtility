#include "GUIEvent.h"
#include "GUIRectangle.h"

GUIEvent::GUIEvent(GUIRectangle *element)
{
  callbackString = element? element->getCallbackString() : "Unknown";
  eventSource    = element;
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
  }
  return *this;
}

GUIRectangle *GUIEvent::getEventSource()   { return eventSource ;   }
const String &GUIEvent::getCallbackString(){ return callbackString; }
