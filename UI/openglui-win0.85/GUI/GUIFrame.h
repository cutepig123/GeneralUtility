#ifndef GUI_FRAME_H
#define GUI_FRAME_H

#include "../Tools/TextureUtils.h"
#include "GUIPanel.h"
typedef vector<GUITexCoordDescriptor> GUITexCoordsInfo;

//class Window;
class GUIFrame : public GUIPanel
{
  private:
    GUIEventListener   *listener;
    GUITexCoordsInfo    texCoords;
    Texture             elementsTexture;
    Tuple2i             windowSize;
    int                 updateCount;
  public:
    GUIFrame();
   ~GUIFrame();

   //    void setParentWindow(Window *parent);
   virtual bool  loadXMLSettings(const TiXmlElement *element);
 
    void     addOrReplaceTexCoordsInfo(GUITexCoordDescriptor &info);
    void     setElementsTexture(const char* texturePath);
    void     setElementsTexture(const Texture &texture);
    void     setGUIEventListener(GUIEventListener *listener);
    Texture *getElementsTexture();

    virtual GUITexCoordDescriptor *getTexCoordsInfo(int type);
    virtual GUIEventListener      *getEventsListener();

    virtual const Tuple4i &getWindowBounds();
    virtual void  forceUpdate(bool update);
    virtual void  enableGUITexture();
    virtual void  disableGUITexture();
    virtual void  render(float tick);
    virtual void  clear();
};

#endif
