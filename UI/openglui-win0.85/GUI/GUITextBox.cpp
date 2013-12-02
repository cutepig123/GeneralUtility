#include "GUITextBox.h"
#include "GUIFontManager.h"

GUITextBox::GUITextBox(const char  *callback,
                       const char  *text) : GUIAlphaElement(callback)

{
  setBordersColor(0.3f, 0.3f, 0.3f);
  setDimensions(80, 18);
  setPadding(2, 2);
  setColor(0.5f, 0.5f, 0.5f);
  label.setString(text);

  blinkerPosition = 2;
  blinkerTimer    = 0.0f;
  blinkerOn       = false;

  textStartIndex  = 0;
  textEndIndex    = 0;

  dimensions.y    = GUIFontManager::getDefaultFont() ?
                    GUIFontManager::getDefaultFont()->getFontObject()->getHeight() + padding.y : dimensions.y;
  widgetType      = TEXT_BOX;
}

bool GUITextBox::loadXMLSettings(const TiXmlElement* element)
{
  if(!XMLArbiter::inspectElementInfo(element, "TextBox"))
    return Logger::writeErrorLog("Need a TextBox node in the xml file");

  setBordersColor(XMLArbiter::fillComponents3f(XMLArbiter::getChildElementByName(element, "BordersColor"),bordersColor));
  return   GUIAlphaElement::loadXMLSettings(element);
return false;
}

void  GUITextBox::setBordersColor(const Tuple3f& color)
{
  setBordersColor(color.x, color.y, color.z);
}

const Tuple3f &GUITextBox::getBordersColor() const
{
  return bordersColor;
}

void  GUITextBox::setBordersColor(float r, float g, float b)
{
  bordersColor.set(clamp(r, 0.0f, 255.0f),
                   clamp(g, 0.0f, 255.0f),
                   clamp(b, 0.0f, 255.0f));
  bordersColor.x /= (bordersColor.x > 1.0) ? 255.0f : 1.0f;
  bordersColor.y /= (bordersColor.y > 1.0) ? 255.0f : 1.0f;
  bordersColor.z /= (bordersColor.z > 1.0) ? 255.0f : 1.0f;
}

void  GUITextBox::setPadding(const Tuple2i& p)
{
  setPadding(p.x, p.y);
}

void  GUITextBox::setPadding(int x, int y)
{
  padding.set(clamp(x, 0, 25),
              clamp(y, 0, 25));
}

const Tuple2i &GUITextBox::getPadding() const
{
  return padding;
}

void GUITextBox::render(float clockTick)
{
  if(!parent || !visible)
    return;

  modifyCurrentAlpha(clockTick);
  getWindowBounds();

  int realBlinkerPosition = clamp(blinkerPosition,
                                  windowBounds.x  + padding.x,
                                  windowBounds.z  - padding.x);
  blinkerTimer += clockTick;
  blinkerTimer -= blinkerTimer > 1.0f ? 1.0f : 0.0f;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4fv(color);

  glBegin(GL_TRIANGLE_STRIP);
  glVertex2i(windowBounds.x, windowBounds.y);
  glVertex2i(windowBounds.x, windowBounds.w);
  glVertex2i(windowBounds.z + padding.x, windowBounds.y);
  glVertex2i(windowBounds.z + padding.x, windowBounds.w);
  glEnd();

  glColor3fv(bordersColor);
  glBegin(GL_LINE_LOOP);
  glVertex2i(windowBounds.x, windowBounds.y);
  glVertex2i(windowBounds.x, windowBounds.w);
  glVertex2i(windowBounds.z + padding.x, windowBounds.w);
  glVertex2i(windowBounds.z + padding.x, windowBounds.y);
  glEnd();
  glDisable(GL_BLEND);

  label.print(windowBounds.x + padding.x, windowBounds.y, textStartIndex, textEndIndex);

  if(blinkerOn && (blinkerTimer > 0.5f))
  {
    glColor3fv(label.getColor());
    glBegin(GL_LINES);
    glVertex2i(realBlinkerPosition, windowBounds.y + padding.y/2);
    glVertex2i(realBlinkerPosition, windowBounds.w - padding.y);
    glEnd();
    glColor3f(1,1,1);
  }
}

void GUITextBox::checkMouseEvents(MouseEvent &newEvent, int extraInfo, bool reservedBits)
{
  bool nRelease   = pressed;

  GUIRectangle::checkMouseEvents(newEvent, extraInfo, true);

  if(mouseOver && (clicked || pressed))
    setupBlinker(newEvent.getX());

  clicked   = false;
  released  = false;
  blinkerOn = focused ? blinkerOn : false;
}

void GUITextBox::checkKeyboardEvents(KeyEvent evt, int extraInfo)
{
  if(!focused || !active)
    return;

  if((evt.getKeyID() >= 32 && evt.getKeyID() <= 127) ||
     (evt.getKeyID() == 8));
  else
    extraInfo = 0;

  if(extraInfo == KEY_PRESSED)
  {
    int length = label.getString().getLength();

    if(evt.displayable())
      setupText(INSERT_CHAR, evt.getKeyChar());

    if(length)
    {
      switch(evt.getKeyID())
      {
        case   8:  setupText(BACKSPACE_DELETE); break;
        case 127:  setupText(SIMPLE_DELETE);    break;
      }
    }
  }

  label.forceUpdate(true);
  update = true;
}

void    GUITextBox::setupBlinker(int x)
{
  if(!focused || !active)
    return;

  GUIFont *font           = GUIFontManager::getFont(label.getFontIndex());
  const    String &string = label.getString();
  const    int    *spaces = NULL;
  blinkerPosition         = getWindowBounds().x + padding.x;
  x -= 1;

  if(font)
  {
     spaces = font->getFontObject()->getCharHorizontalGlyphs();

     for(size_t i = 0; i < string.getLength(); i++)
     if(blinkerPosition < x)
       blinkerPosition += spaces[int(string.getBytes()[i])];
  }

  blinkerOn = true;
}

void GUITextBox::setupText(int type, char Char)
{
  GUIFont *font           = GUIFontManager::getFont(label.getFontIndex());
  const    int    *spaces = font ? font->getFontObject()->getCharHorizontalGlyphs() : NULL,
                   length = label.getString().getLength();
  String   temp;
  int      start  = windowBounds.x + padding.x,
           index  = 0;

  if(!spaces)
    return;

  for(int t = 0; t < length; t++)
  {
    if(blinkerPosition > start)
    {
      index++;
      start += spaces[label.getCharString()[t]];
    }
  }

  if(type == INSERT_CHAR)
  {
    if(index != length && length)
    {
      String leftSide;
      leftSide.set(label.getCharString(), index);
      leftSide += Char;
      temp.set(label.getCharString() + index, length - index);
      label.setString(leftSide + temp);
    }
    else
    {
      temp  = label.getString();
      temp += Char;
      label.setString(temp);
    } 
    blinkerPosition  = blinkerPosition + GUIFontManager::getCharacterWidth(Char, font);
  } 

  if(type == BACKSPACE_DELETE && (blinkerPosition != windowBounds.x + padding.x))
  {
    if(index != length)
    {
      String leftSide;
      setupBlinker(blinkerPosition - GUIFontManager::getCharacterWidth(label.getCharString()[index -1],
                                                                       label.getFontIndex()));

      leftSide.set(label.getCharString(), index - 1);
      temp.set(label.getCharString() + index, length - index);
      label.setString(leftSide + temp);
      return;
    }

    setupBlinker(blinkerPosition - GUIFontManager::getCharacterWidth(label.getCharString()[length -1],
                                                                     font));

    temp.set(label.getCharString(), length - 1);
    if(temp.getLength())
      label.setString(temp);
    else
    {
      label.clear();
      blinkerPosition = windowBounds.x + padding.x;
    }
  }
  
  if(type == SIMPLE_DELETE && length)
  {
    if((blinkerPosition == windowBounds.x + padding.x) && (length == 1))
    {
      label.clear();
      return;
    }

    if(index < length)
    {
      String leftSide;
      leftSide.set(label.getCharString(), index);
      temp.set(label.getCharString() + index + 1, length - index - 1);
      label.setString(leftSide + temp);
    }
  }
  if(type == PARSE_VISIBLE)
     textEndIndex = font->getFontObject()->getMaxFittingLength(label.getString(), getWidth());
}

const Tuple4i &GUITextBox::getWindowBounds()
{
  if(parent && update)
  {
    label.computeDimensions();
    dimensions.y    = label.getHeight() ? label.getHeight() + padding.y : dimensions.y;

    GUIRectangle::computeWindowBounds();
    blinkerPosition = !blinkerPosition ? windowBounds.x + padding.x : blinkerPosition;
    setupText(PARSE_VISIBLE);
  }
  return windowBounds;
}
