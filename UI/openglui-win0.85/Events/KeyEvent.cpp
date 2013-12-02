#include "KeyEvent.h"

KeyEvent::KeyEvent(int keyID)
{
  id = keyID;
}

KeyEvent::KeyEvent(const KeyEvent& copy)
{
  this->operator =(copy);
}

KeyEvent &KeyEvent::operator= (const KeyEvent& copy)
{
  if(this != &copy)
  {
    id = copy.id;
  }
  return *this;
}

bool KeyEvent::displayable()
{
  return id >= 32 && id <= 126;
}

char KeyEvent::getKeyChar(){ return char(id); }
int  KeyEvent::getKeyID()  { return id      ; }
