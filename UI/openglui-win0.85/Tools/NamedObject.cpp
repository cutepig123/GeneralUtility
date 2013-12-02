#include "NamedObject.h"

NamedObject::NamedObject(const char*  argName)
{
  name = argName;
}

NamedObject::NamedObject(const String &argName)
{
  name = argName;
}

NamedObject::NamedObject(const NamedObject &copy)
{
  if(this != &copy)
    name = copy.name;
}

NamedObject &NamedObject::operator=(const NamedObject &copy)
{
  if(this != &copy)
    name = copy.name;
  return *this;
}

void NamedObject::setName(const char   *nameArg)
{
  name = nameArg;
}

void NamedObject::setName(const String &nameArg)
{
  name = nameArg;
}

const String &NamedObject::getName() const
{
  return name;
}

const char *NamedObject::getCharName() const
{
  return  name.getBytes();
}

NamedObject::~NamedObject()
{
  name.clear();
}


