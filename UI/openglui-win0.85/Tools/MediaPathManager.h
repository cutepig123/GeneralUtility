#ifndef MEDIAPATHMANAGER_H
#define MEDIAPATHMANAGER_H

#include "Logger.h"

using namespace std;

class MediaPathManager
{
  public:
    static const char  *getPathAt(int index);
    static int          getPathCount();
    static bool         registerPath(const TiXmlElement *MediaPathNode);
    static bool         registerPath(const char *path);
    static void         printAllPaths();
    static const char*  lookUpMediaPath(const char *path);

};

#endif
