#ifndef GUI_COMBOBOX_H
#define GUI_COMBOBOX_H

#include "GUIButton.h"
#include "GUIPanel.h"
#include "GUILabel.h"

class GUIComboBox : public GUIRectangle, public GUIEventListener
{
  public:
    GUIComboBox(const char *callbackString = NULL);
   ~GUIComboBox();

    virtual GUIEventListener  *getEventsListener();
    virtual const void         computeWindowBounds();
    virtual bool               loadXMLSettings(const TiXmlElement *node);
    virtual void               render(float clockTick);
    virtual void               checkMouseEvents(MouseEvent &evt, int extraInfo, bool reservedBits = false);
    virtual void               actionPerformed(GUIEvent &evt);

    void                    setFontScales(float w, float h);
    void                    setFontScales(const Tuple2f &scales);
    const   Tuple2f        &getFontScales();

    void                    addItem(const String &item);
    const   vector<String> &getItems() const;
    const   char*           getSelectedItem()  const;
    const   char*           getItem(size_t index) const;
    int                     getItemIndex(const String &item);

    void    setScrollingColor(float r, float g, float b, float alpha);
    void    setScrollingColor(const Tuple4f &rgba);
    const   Tuple4f &getScrollingColor() const;

    void    enableScrollingRectangle(bool);
    bool    scrollingRectangleEnabled();

    bool    isDeployed();

  private:

    void           finalizeSize();

    vector<String> items;
    GUIButton      *dropMenuButton;
    GUIPanel       *upperPanel,
                   *lowerPanel;
    GUILabel       *currentSelection;

    Tuple4i         scrollingRectangle;
    Tuple4f         scrollingColor;
    Tuple2f         fontScales;                   

    bool            displayScrollingRectangle,
                    lockItems,
                    deployed;

    int             selectionIndex,
                    fontIndex;
};

#endif
