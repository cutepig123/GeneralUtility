#include "Logger.h"

vector<String> Logger::logStrings;
String         Logger::logPath; 

void Logger::initialize(const char* logfilename)
{
  logPath     = !logfilename ? "Log.txt" : logfilename;
  ofstream logFile(logPath);
  logFile.close();
}

void Logger::flush()
{
  if(!logPath.getLength() || !logStrings.size())
    return;
 
  ofstream logFile(logPath, ios::app);

  for(size_t t = 0; t < logStrings.size(); t++)
    logFile << logStrings[t];

  logStrings.clear();
  logFile.close();
}

void Logger::writeImmidiateInfoLog(const String &info)
{
  if(info.getLength())
  {
    logStrings.push_back(String("<+>") + info + "\n");
    flush();
  }
}

void Logger::writeInfoLog(const String &info)
{
  logStrings.push_back(String("<+>") + info + "\n");
  if(logStrings.size() >= 10)
    flush();
}

bool Logger::writeErrorLog(const String &info)
{
  if(info.getLength())
  {
    logStrings.push_back(String("<!>") + info + "\n");
    flush();
  }
  return false;
}

void Logger::writeFatalErrorLog(const String &info)
{
  if(info.getLength())
  {
   logStrings.push_back(String("<X>") + info + "\n");

    flush();
  }
  exit(1);
}