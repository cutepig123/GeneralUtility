#ifndef GUI_RADIO_BUTTON_H
#define GUI_RADIO_BUTTON_H

#include "GUICheckBox.h"

class GUIRadioButton : public GUICheckBox
{
  public:
    GUIRadioButton(const char  *callbackString = NULL);
    virtual void checkMouseEvents(MouseEvent &newEvent, int extraInfo, bool bits);
};

#endif
