#ifndef MOUSEEVENT_H
#define MOUSEEVENT_H

enum MouseEvents
{
  NOBUTTON,    BUTTON1,      BUTTON2, BUTTON3,
  NOSCROLLING, SCROLLING_UP, SCROLLING_DOWN
};


class MouseEvent
{
  public:
    MouseEvent();
    MouseEvent(int id, int xMouse, 
               int yMouse, int yInvMouse, 
               int scrolling = NOSCROLLING);
    MouseEvent(const MouseEvent& copy);
    MouseEvent &operator= (const MouseEvent& copy);

    int  getScrolling();
    int  getButtonID();
    int  getX();
    int  getY();
    int  getYInverse();

    void consume();
  private:
    int  scrolling,
         buttonID,
         x,
         y,
         yInverse;
};

#endif
