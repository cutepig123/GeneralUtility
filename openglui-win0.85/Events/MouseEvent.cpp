#include "MouseEvent.h"


MouseEvent::MouseEvent()
{
  consume();
}

MouseEvent::MouseEvent(int bID, int xMouse, int yMouse, int invYMouse, int scroll)
{
  scrolling = scroll,
  buttonID  = bID;
  x         = xMouse;
  y         = yMouse;
  yInverse  = invYMouse;
}

MouseEvent::MouseEvent(const MouseEvent& copy)
{
  this->operator =(copy);
}

MouseEvent &MouseEvent::operator= (const MouseEvent& copy)
{
  if(this != &copy)
  {
    scrolling = copy.scrolling;
    buttonID  = copy.buttonID;
    yInverse  = copy.yInverse;
    x         = copy.x;
    y         = copy.y;
  }
  return *this;
}

void MouseEvent::consume()
{
  scrolling = 0;
  buttonID  = NOBUTTON;
  x         = 0;
  y         = 0;
  yInverse  = 0;  
}

int MouseEvent::getScrolling(){ return scrolling ; }
int MouseEvent::getButtonID() { return buttonID;   }
int MouseEvent::getYInverse() { return yInverse;   }
int MouseEvent::getX()        { return x;          }
int MouseEvent::getY()        { return y;          }
