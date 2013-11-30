#ifndef GUI_FONT_H
#define GUI_FONT_H

#include "../Tools/XMLArbiter.h"
#include "../Tools/Font.h"

class GUIFont : public NamedObject
{
  public:
    GUIFont(const char* path = NULL);
   ~GUIFont();

    CFont *getFontObject();
 
    bool operator ==(const GUIFont &compare);
    bool operator !=(const GUIFont &compare);

    bool loadXMLSettings(const TiXmlElement *node);
    bool build();

  private:
    CFont  font;
};
#endif
