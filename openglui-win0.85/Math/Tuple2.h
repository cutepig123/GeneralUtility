/*
 *Author: Abdul Bezrati
 *Email : abezrati@hotmail.com
 */

#ifndef TUPLE2_H
#define TUPLE2_H
#include <iostream>
using namespace std;

template <class T2>

class Tuple2
{
  public:
    Tuple2()
    {
      x = 0;
      y = 0;
    }

    Tuple2(T2 X, T2 Y)
    {
      x = X;
      y = Y;
    }

    Tuple2(Tuple2* tuple)
    {
      x = tuple->x;
      y = tuple->y;
    }

    Tuple2(const Tuple2 &source)
    {
      x = source.x;
      y = source.y;
    }

    Tuple2(const T2* source){
      x = source[0];
      y = source[1];
    }

    Tuple2(T2* source){
      x = source[0];
      y = source[1];
    }

    void operator = (const Tuple2 &right){
      x = right.x;
      y = right.y;
    }

    inline Tuple2 operator + (const Tuple2 &right){
      return Tuple2(right.x + x, right.y + y);
    }

    inline Tuple2 operator - (const Tuple2 &right){
      return Tuple2(-right.x + x, -right.y + y);
    }


    inline Tuple2 operator * (const T2 scale){
      return Tuple2(x*scale, y*scale);
    }

    inline Tuple2  operator / (const T2 scale){
      if(scale) return Tuple2(x/scale, y/scale);
      return Tuple2(0, 0);
    }

    inline Tuple2 &operator = (const T2* source){
      x = source[0];
      y = source[1];
      return *this;
    }

    inline Tuple2 &operator = (T2* source){
      x = source[0];
      y = source[1];
      return *this;
    }

    inline void operator += (const Tuple2 &right){
      x+=right.x;
      y+=right.y;
    }

    inline void operator -= (const Tuple2 &right){
      x-=right.x;
      y-=right.y;
    }

    inline void operator *= (const T2 scale){
      x*=scale;
      y*=scale;
    }

    inline void operator /= (const T2 scale){
      if(!scale) return;
      x/=scale;
      y/=scale;
    }

    inline operator const T2*() const { return &x; }
    inline operator T2*() { return &x; }   

	  inline const T2  operator[](int i) const { return ((T2*)&x)[i]; }
	  inline       T2 &operator[](int i)       { return ((T2*)&x)[i]; }

    bool operator == (const Tuple2 &right){
      return (x == right.x &&
              y == right.y);
    }

    bool operator != (const Tuple2 &right){
      return !(x == right.x &&
               y == right.y );
    }

    void set(T2 nx, T2 ny){
      x = nx;
      y = ny;
    }

    inline void negate(){
      x = -x;
      y = -y;
    }


    inline void clamp(T2 min, T2 max){
      x = x > max ? max : x < min ? min  : x;
      y = y > max ? max : y < min ? min  : y;
    }

    friend ostream & operator<< ( ostream & out, const Tuple2 & right){
      return out << "Tuple2( " << right.x
                               << ", "
                               << right.y
                               << ")\n";
    }

    T2 x, y;
};

typedef Tuple2<int   > Tuple2i;
typedef Tuple2<float > Tuple2f;
typedef Tuple2<double> Tuple2d;

#endif
