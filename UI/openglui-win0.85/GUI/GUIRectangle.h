#ifndef GUI_RECTANGLE_H
#define GUI_RECTANGLE_H

#include "../Tools/XMLArbiter.h"

#include "GUITexCoordDescriptor.h"
#include "../Math/MathUtils.h"
#include "../Tools/Logger.h"

#include "../Events/GUIEventListener.h"
#include "../Events/MouseEvent.h"
#include "../Events/KeyEvent.h"

enum WidgetEvents
{
  CLICKED = 1,
  DOUBLE_CLICKED,
  PRESSED,
  DRAGGED,
  RELEASED,
  MOVED
};

enum  AnchorPoints
{
  CENTER,
  CORNERLU,
  CORNERRU,
  CORNERLD,
  CORNERRD
};

class GUIRectangle
{
  protected:
    GUIRectangle *parent;

    Tuple4i       windowBounds;
    Tuple2f       position,
                  dimensions;

    String        callbackString;

    bool          mouseOver,
                  released,
                  focused,
                  pressed,
                  clicked,
                  visible,
                  active,
                  update;

    int           lastAction,
                  widgetType,
                  anchor,
                  z;                  

  public:
    GUIRectangle(const char *callback = NULL);

    void           setCallbackString(const String& callback);
    void           setCallbackString(const char*   callback);
    const String & getCallbackString();   
   
    bool     loadXMLSettings(const TiXmlElement *node);

    void     setDimensions(float width, float height);
    void     setDimensions(const Tuple2f &dimensions);
    const    Tuple2f &getDimensions();

    void     setPosition(float xScaleOrPosition, float yScaleOrPosition);
    void     setPosition(const Tuple2f &scalesOrPosition);
    const    Tuple2f &getPosition();
    virtual  Tuple2i  getCenter();

    int   getWidth();
    int   getHeight();

    void  setActive(bool active);
    bool  isActive();

    void  setAnchorPoint(const char *anchor);
    void  setAnchorPoint(int anchor);
    int   getAnchorPoint();

    virtual void  forceUpdate(bool update);

    void  setVisible(bool active);
    bool  isVisible();

    bool  isAttached();
    int   getWidgetType();

    virtual void checkMouseEvents(MouseEvent  &evt, int extraInfo, bool reservedBits = false);
    virtual void checkKeyboardEvents(KeyEvent  evt, int extraInfo);
    virtual void render(float) = 0;

    virtual void          setParent(GUIRectangle *parent);
    virtual GUIRectangle *getParent();

    virtual  GUITexCoordDescriptor *getTexCoordsInfo(int type);
    virtual  GUIEventListener      *getEventsListener();

    virtual void  enableGUITexture();
    virtual void  disableGUITexture();

    virtual void  setZCoordinate(int z);
    virtual int   getZCoordinate();

    virtual const void     computeWindowBounds();
    virtual const Tuple4i &getWindowBounds();

    bool  eventDetected();
    bool  isMouseOver();
    bool  isReleased();
    bool  isFocused();
    bool  isDragged();
    bool  isClicked();
    bool  isPressed();
};

#endif
