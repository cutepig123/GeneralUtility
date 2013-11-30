#include "Headers.h"
#include <GL/glut.h>
#include "../Tools/TinyXML.h"

EventsHandler handler;
FPSCounter    counter;
GUILabel     *fpsDisplay;
GUIFrame      guiFrame;

float red   = 0.0f,
      blue  = 0.0f,
      green = 0.0f;

void printingPath(const TiXmlElement *element)
{
  if(!element)
    return;

 
  
  for(const TiXmlAttribute* attribute = element->FirstAttribute();	
      attribute;
   	  attribute = attribute->Next())
  {
   const char* value = attribute->Value();
    if(value)
      cout << value << endl;

  }

}
void iterateNames(const TiXmlElement *element)
{
  if(!element)
   return;

   for(const TiXmlElement *child = element;	
      child;
   	  child = child->NextSiblingElement() )
  {
    const char * value = child->Value();
    if(value && !strcmp(value, "Panel"))
      printingPath(element);
  
 //   iterateNames(child->FirstChildElement());
 
  }


}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(700, 500);
  glutCreateWindow("GUI Test");

  initializeScene();

  // Keyboard
  glutKeyboardFunc(keyTyped);
  glutSpecialFunc(inputKey);

  // Display and Idle
  glutDisplayFunc(renderScene);
  glutIdleFunc(renderScene);

  glutReshapeFunc(setPerspective);

  // Mouse stuff
  glutMouseFunc(mouseClicked);
  glutMotionFunc(mouseDragged);
  glutPassiveMotionFunc(mouseMoved);

  glutMainLoop();
  TexturesManager::flushAllTextures();
  return(0);
}

void initializeScene()
{
  Logger::initialize();
  GLeeInit();
  MediaPathManager::registerPath("Data/Textures/");
  MediaPathManager::registerPath("Data/XML/");
  MediaPathManager::registerPath("Data/GUI/");
  guiFrame.GUIPanel::loadXMLSettings("GUILayout.xml");
  guiFrame.setGUIEventListener(&handler);
/*
	TiXmlDocument doc("Data/GUI/ColorSliders.xml");
bool   loadOkay = doc.LoadFile();
	if ( !loadOkay )
	{
		printf( "Could not load test file 'demotest.xml'. Error='%s'. Exiting.\n", doc.ErrorDesc() );
 	}
  TiXmlElement * parent = doc.FirstChildElement();

 iterateNames(parent);
 
*/

  fpsDisplay = (GUILabel*)guiFrame.getWidgetByCallbackString("fpsCounter");

  Logger::flush();

  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
}

void renderScene()
{
  counter.markFrameStart();
  glClearColor(red, green, blue, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  enter2DMode(guiFrame.getWidth(), guiFrame.getHeight());
  guiFrame.render(counter.getFrameInterval());
  exit2DMode();

  if(fpsDisplay)
     fpsDisplay->setLabelString(String("Current FPS: ") + int(counter.getFPS()));

  glutSwapBuffers();
  counter.markFrameEnd();
}

void EventsHandler::actionPerformed(GUIEvent &evt)
{
  const String &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int           widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == CHECK_BOX)
  {
    GUICheckBox   *checkBox = (GUICheckBox*)sourceRectangle;
  }

  if(widgetType == SLIDER)
  {
    GUISlider  *slider = (GUISlider*)sourceRectangle;
    if(callbackString == "s1")
      slider->setLabelString(String("Slider 1: ") + int(slider->getProgress()*10));

    if(callbackString == "s2")
      slider->setLabelString(String("Slider 2: ") + int(slider->getProgress()*10));

    if(callbackString == "red")   red   = slider->getProgress();
    if(callbackString == "blue")  blue  = slider->getProgress();
    if(callbackString == "green") green = slider->getProgress();
  }

  if(widgetType == BUTTON)
  {
    GUIButton   *button = (GUIButton*)sourceRectangle;
    if(callbackString == "exit")
      if(button->isClicked())
        exit(0);
  }

  if(widgetType == COMBO_BOX)
  {
    GUIComboBox  *comboBox = (GUIComboBox*)sourceRectangle;
    if(callbackString == "cb1")
    if(comboBox->getSelectedItem() == String("Select this item to exit"))
      exit(0);
  }

  if(widgetType == RADIO_BUTTON)
  {
    GUIRadioButton   *radioButton = (GUIRadioButton*)sourceRectangle;
  }

}

void enter2DMode(GLint winWidth, GLint winHeight)
{
  Tuple4i  viewport;

  if(winWidth <= 0 || winHeight <= 0)
  {
    glGetIntegerv(GL_VIEWPORT, viewport);
    winWidth  = viewport.z;
    winHeight = viewport.w;
  }

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, winWidth, winHeight, 0);
  glDisable(GL_DEPTH_TEST);
}

void exit2DMode()
{
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glEnable(GL_DEPTH_TEST);
}

void setPerspective( int width, int height)
{
  float aspectRatio = 1.33f;
  height            = height <= 0 ? 1 : height;
  aspectRatio       = (float)width/height;

  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);

  glLoadIdentity();
  gluPerspective(90.0f, aspectRatio, 1.0f, 1500.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  guiFrame.setDimensions(float(width), float(height));
  guiFrame.forceUpdate(true);
}

void mouseClicked(int button, int state, int x, int y)
{
  button  = (button == GLUT_LEFT_BUTTON ) ? BUTTON1 :
            (button == GLUT_RIGHT_BUTTON) ? BUTTON2 : BUTTON3;

  if (state == GLUT_DOWN){
    MouseEvent event = MouseEvent(BUTTON1, x, y, guiFrame.getHeight() - y);
    guiFrame.checkMouseEvents(event, CLICKED);
  }
  else{
    MouseEvent event = MouseEvent(BUTTON1, x, y, guiFrame.getHeight() - y);
    guiFrame.checkMouseEvents(event, RELEASED);
  }
}

void mouseDragged(int x, int y)
{
  MouseEvent event = MouseEvent(NOBUTTON, x, y, guiFrame.getHeight() - y);
  guiFrame.checkMouseEvents(event, DRAGGED);
}

void mouseMoved(int x, int y)
{
  MouseEvent event = MouseEvent(NOBUTTON, x, y, guiFrame.getHeight() - y);
  guiFrame.checkMouseEvents(event, MOVED);
}

void keyTyped(unsigned char key, int x, int y)
{
  guiFrame.checkKeyboardEvents(KeyEvent(key), KEY_PRESSED);
  if (key == 27)
    exit(0);
}

void inputKey(int key, int x, int y)
{
}
