#include "MediaPathManager.h"
#include "Benchmark.h"
#include "Logger.h"

Benchmark::Benchmark(const char* logFilePath) : FPSCounter() , IOXMLObject("Benchmark")
{
  setLogFilePath(logFilePath);
  framesCounter  =   0.0f;
  elapsedTime    =   0.0f;
  averageFPS     =   0.0f;
  duration       =  10.0f;
  enabled        =   true;
  minFPS         = 10.0e5;
  maxFPS         =   0.0f;
}

bool   Benchmark::loadXMLSettings(const TiXmlElement *element)
{
  if(!XMLArbiter::inspectElementInfo(element, "Benchmark"))
    return  Logger::writeErrorLog("NULL Benchmark node");

  setLogFilePath(element->Attribute("logFilePath"));
  setDuration(XMLArbiter::fillComponents1f(element, "duration", duration));
  setEnabled(XMLArbiter::analyzeBooleanAttr(element, "enabled", enabled));
  return true;
}

bool Benchmark::exportXMLSettings(const char* xmlPath)
{
  if(!xmlPath)
  {
    if(!logFilePath.getLength())
      return Logger::writeErrorLog("Cannot export Benchmark results -> NULL log file path");
  }
  else
    logFilePath = xmlPath;

  ofstream exporter(logFilePath);

  exporter << "<BMResults  framesCount = \"" << framesCounter << "\"\n"
           << "            elapsedTime = \"" << elapsedTime   << "\"\n"
           << "            duration    = \"" << duration      << "\"\n"
           << "            averageFPS  = \"" << averageFPS    << "\"\n"
           << "            minFPS      = \"" << minFPS        << "\"\n"
           << "            maxFPS      = \"" << maxFPS        << "\"/>\n";

  exporter.close();
  return true;
}

void Benchmark::setLogFilePath(const char* lFilePath)
{
  logFilePath = lFilePath ? (String)lFilePath : (String)logFilePath;
}

const String &Benchmark::getLogFilePath() const
{
  return logFilePath;
}

void Benchmark::markFrameEnd()
{

  if(frameStart)
  {
    frameInterval = Timer::getElapsedTimeSeconds(frameStart);
    tickCounter  += frameInterval;
    elapsedTime  += frameInterval*enabled;
    internalFPS++;

    if(tickCounter >= 1.0f)
    {
      fps          = internalFPS/tickCounter;
      internalFPS  = 0.0f;
      tickCounter  = 0.0f;
    }
  }

  framesCounter += 1.0f;
 
  if(elapsedTime >= duration || !enabled)
    return;

  if(fps && elapsedTime)
  {
    averageFPS = framesCounter/elapsedTime;
    minFPS     = (fps < minFPS) ? fps : minFPS;
    maxFPS     = (fps > maxFPS) ? fps : maxFPS;
  }
}

void   Benchmark::setEnabled(bool on)
{
  enabled = on;
}

bool   Benchmark::isEnabled()
{
  return enabled;
}

bool Benchmark::running()
{
  return (elapsedTime < duration);
}

void Benchmark::setDuration(float dur){ duration = clamp(dur, 1.0f, 3600.0f); }

float  Benchmark::getTotalDuration(){ return elapsedTime;       }
float  Benchmark::getAverageFrames(){ return averageFPS;        }
float  Benchmark::getTotalFrames()  { return framesCounter;     }
float  Benchmark::getDuration()     { return duration;          }
float  Benchmark::getMinFPS()       { return minFPS;            }
float  Benchmark::getMaxFPS()       { return maxFPS;            }
