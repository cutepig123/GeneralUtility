#include "FPSCounter.h"

FPSCounter::FPSCounter()
{
  Timer::intialize();
  frameInterval = 0.0f;
  internalFPS   = 0.0f;
  tickCounter   = 0.0f;
  elapsedTime   = 0.0f;
  frameStart    =    0;
  fps           = 0.0f;
}

const float FPSCounter::getFrameInterval() const { return frameInterval; }
const float FPSCounter::getElapsedTime()   const { return elapsedTime;   }
const float FPSCounter::getFPS()           const { return fps;           }

void FPSCounter::markFrameStart()
{
  frameStart = Timer::getCurrentTime();
}

void FPSCounter::markFrameEnd()
{
  if(frameStart)
  {
    frameInterval = Timer::getElapsedTimeSeconds(frameStart);
    tickCounter  += frameInterval;
    elapsedTime  += frameInterval;
    internalFPS++;

    if(tickCounter >= 1.0f)
    {
      fps          = internalFPS/tickCounter;
      internalFPS  = 0.0f;
      tickCounter  = 0.0f;
    }
  }
}
