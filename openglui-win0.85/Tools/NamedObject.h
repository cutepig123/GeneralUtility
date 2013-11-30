#ifndef NAMEDOBJECT_H
#define NAMEDOBJECT_H

#include "String.h"

class NamedObject
{

  protected:
    String name;

  public:
    NamedObject(const char*  argName = NULL);
    NamedObject(const String &argName);
    NamedObject(const NamedObject &copy);
   ~NamedObject();

    NamedObject &operator=(const NamedObject &copy);
    void   setName(const char   *nameArg);
    void   setName(const String &name);

    const String &getName()     const;
    const char*   getCharName() const;

};

#endif
