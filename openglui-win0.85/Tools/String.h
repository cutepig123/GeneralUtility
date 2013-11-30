#ifndef STRING_H
#define STRING_H

#include <cstring>
#include <cstdio>

class String
{
  public:
    String(const char* string = NULL);
    String(const String &copy);
   ~String();

    operator const char*()    const ;
    const    char *getBytes() const;

    String &operator  =(const String &copy  );
    String &operator  =(const char   *string);

    String &operator +=(const String &copy);
    String  operator + (const String &copy);

    String &operator +=(const char character);
    String  operator + (const char character);

    String  operator + (const int    numberi);
    String  operator + (const float  numberf);

    String &operator +=(const int    numberi);
    String &operator +=(const float  numberf);

    String &addFloat(const float numberf, int precision);

    void set(const char* string, int count);
    void clear();
    const unsigned int getLength() const;

  private:
    unsigned int   length;
    char *data;
};

bool    operator  < (const char   *arg1, const String &arg2);
bool    operator  < (const String &arg1, const char   *arg2);
bool    operator  < (const String &arg1, const String &arg2);
bool    operator  > (const char   *arg1, const String &arg2);
bool    operator  > (const String &arg1, const char   *arg2);
bool    operator  > (const String &arg1, const String &arg2);

bool   operator  ==(const String &arg1 , const String &arg2);
bool   operator  ==(const String &arg  , const char   *bytes);
bool   operator  ==(const char   *bytes, const String &arg);

bool   operator  !=(const String &arg1 , const String &arg2);
bool   operator  !=(const String &arg  , const char   *bytes);
bool   operator  !=(const char   *bytes, const String &arg);

#endif