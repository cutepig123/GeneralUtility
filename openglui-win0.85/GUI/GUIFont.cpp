#include "GUIFont.h"

GUIFont::GUIFont(const char* name) : NamedObject(name)
{

}

bool GUIFont::build()
{
  if(!getName().getBytes())
    return Logger::writeErrorLog("Null font path");
  
  return font.load(getName());
}

bool GUIFont::loadXMLSettings(const TiXmlElement *element)
{
  if(!element || !element->Value() || strcmp(element->Value(),  "Font"))
    return Logger::writeErrorLog("Need a Font node in the xml file");

  setName(element->Attribute("path"));

  return true;
}

CFont *GUIFont::getFontObject()
{
  return &font;
}

bool  GUIFont::operator ==(const GUIFont &compare)
{
  return (name   == compare.getName());  
}

bool  GUIFont::operator !=(const GUIFont &compare)
{
  return !operator ==(compare);  
}

GUIFont::~GUIFont()
{
  font.~CFont();
}
