#include "GUISeparator.h"
#include "GUIPanel.h"

GUISeparator::GUISeparator(int orientation_) : GUIRectangle()
{
  setOrientation(orientation);
  setRatio(1.0f);
  widgetType = SEPARATOR;
}

bool GUISeparator::loadXMLSettings(const TiXmlElement *element)
{
  if(!element || !element->Value() || strcmp(element->Value(),  "Separator"))
    return Logger::writeErrorLog("Need a <Separator> tag");

  setRatio(XMLArbiter::fillComponents1f(element, "ratio", ratio));
  return true;
}

void  GUISeparator::setRatio(float ratio_)
{
  ratio = clamp(ratio_, 0.1f, 1.0f);
} 

float GUISeparator::getRatio()
{
  return ratio;
}

void GUISeparator::setOrientation(int orientation_)
{
  orientation =  orientation_ == HORIZONTAL ? HORIZONTAL : VERTICAL;
}

int  GUISeparator::getOrientation()
{
  return orientation;
}

void GUISeparator::render(float tick)
{
  if(!parent || !((GUIPanel*)parent)->isBGColorOn())
    return;

  glColor3fv(((GUIPanel*)parent)->getBordersColor());

  glBegin(GL_LINES);
  glVertex2i(windowBounds.x, windowBounds.y);
  glVertex2i(windowBounds.z, (orientation == VERTICAL) ? windowBounds.w : windowBounds.y);
  glEnd();
  glColor3f(1.0f, 1.0f, 1.0f);
}

void const GUISeparator::computeWindowBounds()
{
  const Tuple4i &parentBounds = parent->getWindowBounds();
  if(orientation == HORIZONTAL)
  {
    float offset = (float(parent->getWidth()) - float(ratio*parent->getWidth()))/2.0f;
    windowBounds.set(int(parentBounds.x + offset), int(parentBounds.y + position.y),
                     int(parentBounds.z - offset), int(parentBounds.y + position.y));
  }
  else
  {
    float offset = (float(parent->getHeight()) - float(parent->getHeight())*ratio)/2.0f;
    windowBounds.set(int(parentBounds.x + position.x), int(parentBounds.y + offset),
                     int(parentBounds.x + position.x), int(parentBounds.w - offset));
  }
}

const Tuple4i &GUISeparator::getWindowBounds()
{
  windowBounds.set(0,0,0,1);
  return windowBounds;
}

void GUISeparator::checkMouseEvents(MouseEvent  &evt, int extraInfo){}
void GUISeparator::checkKeyboardEvents(KeyEvent  evt, int extraInfo){}