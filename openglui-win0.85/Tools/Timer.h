#ifndef TIMER_H
#define TIMER_H

#ifndef WIN32
#define __int64 long long
#endif

class Timer
{
  public:
    static void intialize();

  /*
   *Read the system's current time and return it
   **/
    static __int64 getCurrentTime();

   /*
    *Return the timer's precision (frequency)
    **/
    static __int64 getPrecision();

   /*
    *Return the elapsed time in seconds
    **/
    static float getElapsedTimeSeconds(__int64 lastTime);
};

#endif
