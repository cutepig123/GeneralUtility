#ifndef HEADERS_H
#define HEADERS_H

#include "../Tools/FPSCounter.h"
#include "../GUI/GUIUtils.h"

#pragma comment(linker, "/ENTRY:mainCRTStartup")
#ifdef _DEBUG
  #pragma comment(linker, "/subsystem:console" )
#endif

void mouseClicked(int button, int state, int x, int y) ;
void mouseDragged(int x, int y);
void mouseMoved(int x, int y);

void keyTyped(unsigned char key, int x, int y);
void inputKey(int key, int x, int y) ;

void initializeScene();
void setPerspective(int width, int height);
void renderScene();
void enter2DMode(int width, int height);
void exit2DMode();


class EventsHandler : public GUIEventListener
{
  public:
    virtual void actionPerformed(GUIEvent &evt);
};

#endif
