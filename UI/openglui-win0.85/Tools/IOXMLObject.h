#ifndef XML_IO_H
#define XML_IO_H

#include "XMLArbiter.h"
#include "NamedObject.h"

class IOXMLObject
{
  public:
    IOXMLObject(const char *ioname);

    virtual bool loadXMLSettings(const char *path  );
    virtual bool loadXMLSettings(const TiXmlElement *element) = 0;

    virtual bool exportXMLSettings(ofstream &xmlFile  ) = 0;
    virtual bool exportXMLSettings(const char* xmlPath);

    bool    isSuitable(const TiXmlElement *element);
  protected:
    String ioname;
};

#endif
