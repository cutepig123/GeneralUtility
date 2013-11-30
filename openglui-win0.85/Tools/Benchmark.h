#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "FPSCounter.h"
#include "XMLArbiter.h"
#include "String.h"

class Benchmark : public FPSCounter, IOXMLObject
{
  private:
    String  logFilePath;
    float   framesCounter,
            elapsedTime,
            averageFPS,
            duration,
            minFPS,
            maxFPS;
     bool   enabled;

  public:
    Benchmark(const char* logFilePath = "Results.xml");

    void    setLogFilePath(const char* logFilePath);
    const   String & getLogFilePath() const;
    void    setDuration(float);
    float   getDuration();

    virtual bool exportXMLSettings(const char* xmlPath);
    virtual bool loadXMLSettings(const TiXmlElement *element);

    bool    exportResults();
    bool    running();
    virtual void  markFrameEnd();

    float  getTotalDuration();
    float  getAverageFrames();
    float  getTotalFrames();
    float  getMinFPS();
    float  getMaxFPS();
    
    void   setEnabled(bool);
    bool   isEnabled();
};
#endif