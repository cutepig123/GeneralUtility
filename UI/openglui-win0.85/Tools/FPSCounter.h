#ifndef FPS_COUNTER_H
#define FPS_COUNTER_H

#include "Timer.h"

class FPSCounter
{
  public:
    FPSCounter();

    const float getFrameInterval() const;
    const float getElapsedTime()   const;
    const float getFPS()           const;

    virtual void  markFrameStart();
    virtual void  markFrameEnd();

  protected:
    __int64  frameStart;       // Mark the beginning of a frame
    float    frameInterval,    //The delay between two frames
             elapsedTime,
             internalFPS,      //An internal FPS counter
             tickCounter,      //This will count the clock ticks
             fps;              //The number of frames per second

};

#endif
