#include "Timer.h"

#ifndef WIN32 // not windows, assuming POSIX
  #include <cstdlib>
  #include <sys/time.h>
  #include <unistd.h>
#define secondsToMicro 1000000

#else // windows
  #include <windows.h>

LARGE_INTEGER currentTime,
              frequency;
#endif


void Timer::intialize()
{
#ifdef WIN32
  QueryPerformanceFrequency(&frequency);
#endif
}


__int64 Timer::getCurrentTime(){
#ifdef WIN32
  QueryPerformanceCounter(&currentTime);
  return currentTime.QuadPart;
#else
  timeval internalTime;
  gettimeofday(&internalTime, NULL);
  return (__int64) internalTime.tv_sec * secondsToMicro + internalTime.tv_usec;
#endif
}

__int64 Timer::getPrecision(){
#ifdef WIN32
  return frequency.QuadPart;
#else
  return secondsToMicro;
#endif
}

float Timer::getElapsedTimeSeconds(__int64 lastTime){
#ifdef WIN32
  return float(getCurrentTime() - lastTime)/frequency.QuadPart;
#else
  return float(getCurrentTime() - lastTime) /secondsToMicro;
#endif
}