#include "GUIRadioButton.h"
#include "GUIPanel.h"

GUIRadioButton::GUIRadioButton(const char* cbs) : GUICheckBox(cbs)
{
  secondaryTexDesc = CHECK_RB_MARK;
  primaryTexDesc   = RADIO_BUTTON;
  widgetType       = RADIO_BUTTON;
  markRatio        = 0.75f;
}

void GUIRadioButton::checkMouseEvents(MouseEvent &newEvent, int extraInfo, bool bits)
{
  GUIRectangle::checkMouseEvents(newEvent, extraInfo, true);

  if(clicked) 
  {
    if(!checked)
    {
      setChecked(true);
     ((GUIPanel*)parent)->notify(this);
    }
  }
  released = false;
  clicked  = false;
}