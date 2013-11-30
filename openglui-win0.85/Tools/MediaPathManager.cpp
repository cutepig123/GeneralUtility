#include "MediaPathManager.h"

vector<String> dataPaths;

const char* MediaPathManager::lookUpMediaPath(const char *path)
{
  if(!path)
    return NULL;

  ifstream test;
  string   buffer;

  test.open(path);

  if(test.is_open())
  {
    test.close();
    return path;
  }

  size_t count = dataPaths.size();
  char   *pathBuffer = new char[256];
  memset(pathBuffer, NULL, 256);

  for(size_t i = 0; i < count; i++)
  {
    buffer  = dataPaths[i];
    buffer += path;

    test.open(buffer.data());
    if(test.is_open())
    {
      strcpy(pathBuffer, buffer.data());
      test.close();
      return pathBuffer;
    }
  }
  deleteArray(pathBuffer);
  return NULL;
}

bool MediaPathManager::registerPath(const TiXmlElement *mediaPathNode)

{
  if(mediaPathNode)
   return  registerPath(mediaPathNode->Attribute("path"));

  return false;
}

bool MediaPathManager::registerPath(const char *path)
{
  if(!path || !strlen(path))
    return Logger::writeErrorLog("Failed to register data path -> NULL");

  for(size_t i = 0; i < dataPaths.size(); i++)
    if(dataPaths[i] == path)
      return true;

  String stringBuffer = path;

  Logger::writeInfoLog(String("Registering data path -> ") + path);
  dataPaths.push_back(stringBuffer);
  return true;
}

int MediaPathManager::getPathCount()
{
  return int(dataPaths.size());
}

const char *MediaPathManager::getPathAt(int index)
{
  if(!dataPaths.size() || index >= int(dataPaths.size()) || index < 0)
    return NULL;
  return dataPaths[size_t(index)];
}

void  MediaPathManager::printAllPaths()
{
  cout << "List of registred Media Paths: \n";

  for(size_t i = 0; i < dataPaths.size(); i++){
    if(dataPaths[i]){
      cout << int(i) << "-" << dataPaths[i] << endl;
    }
  }

  cout << endl;
}