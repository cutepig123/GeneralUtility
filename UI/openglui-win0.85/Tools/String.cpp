#include "String.h"

String::String(const char* string)
{
  length = 0;
  data   = NULL;
  operator=(string);
}

String::String(const String &copy)
{
  data   = NULL;
  length = 0;
  operator=(copy);
}

String &String::operator  =(const String &copy)
{
  if(this != &copy)
  {
    clear();
    if(copy.data && copy.length)
    {
      data   = new char[copy.length + 1];
      length = copy.length;
      memcpy(data, copy.data, copy.length);
      data[length] = NULL;
    }
  }
  return *this;
}

void String::set(const char* string, int slen)
{
  clear();

  if(string && slen)
  {
    data   = new char[slen + 1];
    length = slen;
    memcpy(data, string, slen);
    data[length] = NULL;
  }
}

String &String::operator  =(const char *string)
{
  clear();

  unsigned int slen = 0;
  if(string && (slen = unsigned int (strlen(string))))
  {
    data   = new char[slen + 1];
    length = slen;
    memcpy(data, string, slen);
    data[length] = NULL;
  }
  return *this;
}

String &String::operator +=(const char character)
{
  if(character != NULL)
  {
    if(data && length)
    {
      int   newLength = length + 1;
      char *newBuffer = new char[newLength + 1];

      memcpy(newBuffer, data, length);
      newBuffer[length    ] = character;
      newBuffer[length + 1] = NULL;
      clear();
      length = newLength;
      data   = newBuffer;
    }
    else
    {
      data    = new char[2];
      data[0] = character;
      data[1] =  NULL;
      length  = 1;
    }
  }
  return *this;
}

String  String::operator + (const char character)
{
  String returnString(*this);
  returnString += character;
  return returnString;
}

String &String::operator +=(const String &copy)
{
  if(copy.data && copy.length)
  {
    char *newBuffer = new char[length + copy.length + 1];
    int   newLength = length + copy.length;

    if(!data)
    {
      data   = newBuffer;
      length = newLength;

      memcpy(data, copy.data, length);
      data[length] = NULL;
    }
    else
    {
      memcpy(newBuffer         , data     , length     );
      memcpy(newBuffer + length, copy.data, copy.length);
      newBuffer[newLength] = NULL;
      clear();
      data   = newBuffer;
      length = newLength;
    }
  }
  return *this;
}

String  String::operator + (const String &copy)
{
  String returnString(*this);
  returnString += copy;
  return returnString;
}

String String::operator + (const int numberi)
{
  String  returnString;
  char   *numberBuffer = new char[12 + length];

  if(data && length)
    sprintf(numberBuffer, "%s%d", data, numberi);
  else
    sprintf(numberBuffer, "%d", numberi);

  returnString.data   = numberBuffer;
  returnString.length = unsigned int(strlen(numberBuffer));
  return returnString;
}

String String::operator + (const float numberf)
{
  String  returnString;
  char   *numberBuffer = new char[16 + length];

  if(data && length)
    sprintf(numberBuffer, "%s%.2f", data, numberf);
  else
    sprintf(numberBuffer, "%.2f", numberf);

  returnString.data   = numberBuffer;
  returnString.length = unsigned int(strlen(numberBuffer));
  return returnString;
}

String &String::operator +=(const int    numberi)
{
  char   *numberBuffer = new char[12 + length];

  if(data && length)
    sprintf(numberBuffer, "%s%d", data, numberi);
  else
    sprintf(numberBuffer, "%d", numberi);

  clear();

  data   = numberBuffer;
  length = unsigned int(strlen(numberBuffer));

  return *this;
}

String &String::operator +=(const float  numberf)
{
  char   *numberBuffer = new char[16 + length];

  if(data && length)
    sprintf(numberBuffer, "%s%.2f", data, numberf);
  else
    sprintf(numberBuffer, "%.2f", numberf);

  clear();

  data   = numberBuffer;
  length = unsigned int(strlen(numberBuffer));

  return *this;
}

bool   operator  < (const char   *arg1, const String &arg2)
{
  return (!arg2.getLength() || !arg1) ? false :
         (strcmp(arg1, arg2)  < 0);
}
bool   operator  < (const String &arg1, const char   *arg2)
{
  return (!arg1.getLength() || !arg2) ? false :
         (strcmp(arg1, arg2)  < 0);
}

bool   operator  < (const String &arg1, const String &arg2)
{
  return (!arg1.getLength() || !arg2.getLength()) ? false :
         (strcmp(arg1, arg2)  < 0);
}

bool   operator  > (const char   *arg1, const String &arg2)
{
  return (!arg2.getLength() || !arg1) ? false :
         (strcmp(arg1, arg2)  > 0);
}

bool   operator  > (const String &arg1, const char   *arg2)
{
  return (!arg1.getLength() || !arg2) ? false :
         (strcmp(arg1, arg2)  > 0);
}

bool   operator  > (const String &arg1, const String &arg2)
{
  return (!arg1.getLength() || !arg2.getLength()) ? false :
         (strcmp(arg1, arg2)  > 0);
}

bool   operator  ==(const String &arg1, const String &arg2)
{
  return (arg1.getLength() == arg2.getLength()) &&
         (arg1.getLength() !=                0) &&
         !strcmp(arg1, arg2);
}

bool   operator  ==(const char   *bytes, const String &arg)
{
  return (bytes           != NULL) &&
         (arg.getLength() !=    0) &&
         !strcmp(arg, bytes);
}

bool   operator  ==(const String &arg , const char   *bytes)
{
  return (bytes           != NULL) &&
         (arg.getLength() !=    0) &&
         !strcmp(arg, bytes);
}

bool   operator  !=(const String &arg1, const String &arg2)
{
  return !((arg1.getLength() == arg2.getLength()) &&
           (arg1.getLength() !=                0) &&
           !strcmp(arg1, arg2));
}

bool   operator  !=(const char   *bytes, const String &arg)
{
  return !((bytes           != NULL) &&
           (arg.getLength() !=    0) &&
           !strcmp(arg, bytes));
}

bool   operator  !=(const String &arg , const char   *bytes)
{
  return !((bytes           != NULL) &&
           (arg.getLength() !=    0) &&
           !strcmp(arg, bytes));
}

String::operator const char* () const
{
  return data;
}

const    char *String::getBytes() const
{
  return data;
}

const unsigned int String::getLength() const
{
  return length;
}

void String::clear()
{
  if(data)
  {
    delete [] data;
    data = NULL;
  }
  length = 0;
}

String::~String()
{
  clear();
}

String &String::addFloat(const float numberf, int precision)
{
  precision          =  precision < 1 ? 1 : precision > 6 ? 6 : precision;
  const char *format = (precision == 1) ? "%s%.1f" :
                       (precision == 2) ? "%s%.2f" :
                       (precision == 3) ? "%s%.3f" :
                       (precision == 4) ? "%s%.4f" :
                       (precision == 5) ? "%s%.5f" : "%s%.6f";

  char *numberBuffer = new char[16 + length];
 
  if(data && length)
    sprintf(numberBuffer, format, data, numberf);
  else
    sprintf(numberBuffer, "%s", data);

  clear();

  data   = numberBuffer;
  length = (unsigned int)(strlen(numberBuffer));

  return *this; 
}
