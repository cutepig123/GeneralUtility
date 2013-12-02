#include "IOXMLObject.h"
#include "MediaPathManager.h"

IOXMLObject::IOXMLObject(const char *ionameArg)
{
  ioname = ionameArg;
}

bool IOXMLObject::loadXMLSettings(const char* path)
{
  if(!ioname.getLength())
    return Logger::writeErrorLog("Cannot load XML settings -> NULL element name");

  const char *verified = MediaPathManager::lookUpMediaPath(path);

  if(!verified)
    return Logger::writeErrorLog(String("Couldn't locate the <") +
                                 ioname + "> XML file at \""       +
                                 path   + "\" even with a look up");

	TiXmlDocument xmlStack(verified);

  if(!xmlStack.LoadFile())
    return Logger::writeErrorLog(String("Invalid XML file -> ") + verified);

 
  for(const TiXmlElement *child = xmlStack.FirstChildElement();	
      child;
   	  child = child->NextSiblingElement() )
    if(ioname == child->Value())
      return loadXMLSettings(child);
  
  return Logger::writeErrorLog(String("Need a <") + ioname + "> tag in the XML file");
}

bool IOXMLObject::exportXMLSettings(const char* xmlPath)
{
  ofstream xmlFile(xmlPath, ios::app | ios::binary);
  bool result = exportXMLSettings(xmlFile);
  xmlFile.close();
  return result;
}

bool IOXMLObject::isSuitable(const TiXmlElement *element)
{
  if(!element)
    return  Logger::writeErrorLog(String("NULL <") + ioname +  "> node");

  if(ioname != element->Value())
    return Logger::writeErrorLog(String("Need a <") + ioname + "> tag in the XML Node");

  return true;
}


