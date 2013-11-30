#include "XMLArbiter.h"

Tuple4f &XMLArbiter::fillComponents4f(const TiXmlElement *element, Tuple4f &tuple)
{
  if(element)
  for(const TiXmlAttribute *attribute = element->FirstAttribute();
      attribute;
      attribute = attribute->Next())
  {
    const char *name  = attribute->Name(),
               *value = attribute->Value();
    
    if(name && value)
    switch(name[0])
    {
      case 'x': case 'X': case 'r': case 'R': case 's': case 'S': 
        tuple.x = float(atof(value));
      break;

      case 'y': case 'Y': case 'g': case 'G': case 't': case 'T': 
        tuple.y = float(atof(value));
      break;

      case 'z': case 'Z': case 'b': case 'B': case 'u': case 'U': 
        tuple.z = float(atof(value));
      break;

      case 'w': case 'W': case 'a': case 'A': case 'v': case 'V': 
        tuple.w = float(atof(value));
      break;
    }
  }
  return tuple;
}

Tuple4i &XMLArbiter::fillComponents4i(const TiXmlElement *element, Tuple4i &tuple)
{
  if(element)
  for(const TiXmlAttribute *attribute = element->FirstAttribute();
      attribute;
      attribute = attribute->Next())
  {
    const char *name  = attribute->Name(),
               *value = attribute->Value();
    
    if(name && value)
    switch(name[0])
    {
      case 'x': case 'X': case 'r': case 'R': case 's': case 'S': 
        tuple.x = atoi(value);
      break;

      case 'y': case 'Y': case 'g': case 'G': case 't': case 'T': 
        tuple.y = atoi(value);
      break;

      case 'z': case 'Z': case 'b': case 'B': case 'u': case 'U': 
        tuple.z = atoi(value);
      break;

      case 'w': case 'W': case 'a': case 'A': case 'v': case 'V': 
        tuple.w = atoi(value);
      break;
    }
  }
  return tuple;
}

Tuple4d &XMLArbiter::fillComponents4d(const TiXmlElement *element, Tuple4d &tuple)
{
  if(element)
  for(const TiXmlAttribute *attribute = element->FirstAttribute();
      attribute;
      attribute = attribute->Next())
  {
    const char *name  = attribute->Name(),
               *value = attribute->Value();
    
    if(name && value)
    switch(name[0])
    {
      case 'x': case 'X': case 'r': case 'R': case 's': case 'S': 
        tuple.x = atof(value);
      break;

      case 'y': case 'Y': case 'g': case 'G': case 't': case 'T': 
        tuple.y = atof(value);
      break;

      case 'z': case 'Z': case 'b': case 'B': case 'u': case 'U': 
        tuple.z = atof(value);
      break; 

      case 'w': case 'W': case 'a': case 'A': case 'v': case 'V': 
        tuple.w = atof(value);
      break;
    }
  }
  return tuple;
}


Tuple3f &XMLArbiter::fillComponents3f(const TiXmlElement *element, Tuple3f &tuple)
{
  if(element)
  for(const TiXmlAttribute *attribute = element->FirstAttribute();
      attribute;
      attribute = attribute->Next())
  {
    const char *name  = attribute->Name(),
               *value = attribute->Value();
    
    if(name && value)
    switch(name[0])
    {
      case 'x': case 'X': case 'r': case 'R': case 's': case 'S': 
        tuple.x = float(atof(value));
      break;

      case 'y': case 'Y': case 'g': case 'G': case 't': case 'T': 
        tuple.y = float(atof(value));
      break;

      case 'z': case 'Z': case 'b': case 'B': case 'u': case 'U': 
        tuple.z = float(atof(value));
      break;
    }
  }
  return tuple;
}

Tuple3i &XMLArbiter::fillComponents3i(const TiXmlElement *element, Tuple3i &tuple)
{
  if(element)
  for(const TiXmlAttribute *attribute = element->FirstAttribute();
      attribute;
      attribute = attribute->Next())
  {
    const char *name  = attribute->Name(),
               *value = attribute->Value();
    
    if(name && value)
    switch(name[0])
    {
      case 'x': case 'X': case 'r': case 'R': case 's': case 'S': 
        tuple.x = atoi(value);
      break;

      case 'y': case 'Y': case 'g': case 'G': case 't': case 'T': 
        tuple.y = atoi(value);
      break;

      case 'z': case 'Z': case 'b': case 'B': case 'u': case 'U': 
        tuple.z = atoi(value);
      break;
    }
  }
  return tuple;
}

Tuple3d &XMLArbiter::fillComponents3d(const TiXmlElement *element, Tuple3d &tuple)
{
  if(element)
  for(const TiXmlAttribute *attribute = element->FirstAttribute();
      attribute;
      attribute = attribute->Next())
  {
    const char *name  = attribute->Name(),
               *value = attribute->Value();
    
    if(name && value)
    switch(name[0])
    {
      case 'x': case 'X': case 'r': case 'R': case 's': case 'S': 
        tuple.x = atof(value);
      break;

      case 'y': case 'Y': case 'g': case 'G': case 't': case 'T': 
        tuple.y = atof(value);
      break;

      case 'z': case 'Z': case 'b': case 'B': case 'u': case 'U': 
        tuple.z = atof(value);
      break;
    }
  }
  return tuple;
}

Tuple2f &XMLArbiter::fillComponents2f(const TiXmlElement *element, Tuple2f &tuple)
{
  if(element)
  for(const TiXmlAttribute *attribute = element->FirstAttribute();
      attribute;
      attribute = attribute->Next())
  {
    const char *name  = attribute->Name(),
               *value = attribute->Value();
    
    if(name && value)
    switch(name[0])
    {
      case 'x': case 'X': case 'r': case 'R': case 's': case 'S': 
        tuple.x = float(atof(value));
      break;

      case 'y': case 'Y': case 'g': case 'G': case 't': case 'T': 
        tuple.y = float(atof(value));
      break;
    }
  }
  return tuple;
}

Tuple2i &XMLArbiter::fillComponents2i(const TiXmlElement *element, Tuple2i &tuple)
{
  if(element)
  for(const TiXmlAttribute *attribute = element->FirstAttribute();
      attribute;
      attribute = attribute->Next())
  {
    const char *name  = attribute->Name(),
               *value = attribute->Value();
    
    if(name && value)
    switch(name[0])
    {
      case 'x': case 'X': case 'r': case 'R': case 's': case 'S': 
        tuple.x = atoi(value);
      break;

      case 'y': case 'Y': case 'g': case 'G': case 't': case 'T': 
        tuple.y = atoi(value);
      break;
    }
  }
  return tuple;
}

Tuple2d &XMLArbiter::fillComponents2d(const TiXmlElement *element, Tuple2d &tuple)
{
  if(element)
  for(const TiXmlAttribute *attribute = element->FirstAttribute();
      attribute;
      attribute = attribute->Next())
  {
    const char *name  = attribute->Name(),
               *value = attribute->Value();
    
    if(name && value)
    switch(name[0])
    {
      case 'x': case 'X': case 'r': case 'R': case 's': case 'S': 
        tuple.x = atof(value);
      break;

      case 'y': case 'Y': case 'g': case 'G': case 't': case 'T': 
        tuple.y = atof(value);
      break;
    }
  }
  return tuple;
}


double  XMLArbiter::fillComponents1d(const TiXmlElement *element,
                                     const char         *attr,
                                     double              valueArg)
{
  double value = valueArg;

  if(element && attr)
  if(element->Attribute(attr, &value))
    return value;

  return valueArg;
}

float   XMLArbiter::fillComponents1f(const TiXmlElement *element,
                                     const char         *attr, 
                                     float               valueArg)
{
  double value = valueArg;

  if(element && attr)
  if(element->Attribute(attr, &value))
    return float(value);

  return valueArg;
}

int     XMLArbiter::fillComponents1i(const TiXmlElement *element,
                                     const char         *attr, 
                                     int                 valueArg)
{
  int value = valueArg;

  if(element && attr)
  if(element->Attribute(attr, &value))
    return value;

  return valueArg;
}


bool XMLArbiter::analyzeBooleanAttr(const TiXmlElement *element,
                                    const char         *attr, 
                                    bool                source)
{
  if(element && attr)
  for(const TiXmlAttribute *attribute = element->FirstAttribute();
      attribute;
      attribute = attribute->Next())
  {
    const char *name  = attribute->Name(),
               *value = attribute->Value();
    
    if(name && !strcmp(name, attr) && value)
    {
      source = !strcmp(value, "true");
      break;
    }
  }
  return source;
}

const TiXmlElement *XMLArbiter::getChildElementByName(const TiXmlElement *element,
                                                      const char         *name)
{
  if(element && name)
  for(const TiXmlElement *child = element->FirstChildElement();	
      child;
   	  child = child->NextSiblingElement() )
  {
    const char *value = child->Value();
    if(value && !strcmp(name, value))
      return child;
  }
  return NULL;
}

bool XMLArbiter::inspectElementInfo(const TiXmlElement *element,
                                    const char         *desiredType)
{
  if(!element || !desiredType || !element->Value() || strcmp(element->Value(), desiredType))
    return false;

  return true; 
}