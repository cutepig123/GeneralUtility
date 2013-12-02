/*
 *Author: Abdul Bezrati
 *Email : abezrati@hotmail.com
 */

#ifndef TUPLE4
#define TUPLE4


#include <iostream>
#include "Tuple3.h"

using namespace std;

template <class type>

class Tuple4
{
  public:
    Tuple4()
    {
      x = 0;
      y = 0;
      z = 0;
      w = 0;
    }

    Tuple4(type X, type Y, type Z, type W)
    {
      x = X;
      y = Y;
      z = Z;
      w = W;
    }

    Tuple4(type XYZW)
    {
      x =  
      y =  
      z =  
      w = XYZW;
    }

    Tuple4(Tuple4* tuple){
      x = tuple->x;
      y = tuple->y;
      z = tuple->z;
      w = tuple->w;
    }

    Tuple4(const Tuple4<type> &source){
      x = source.x;
      y = source.y;
      z = source.z;
      w = source.w;
    }
 
    Tuple4(const Tuple3<type> &source){
      x = source.x;
      y = source.y;
      z = source.z;
      w = 1.0f;
    }

    Tuple4(const Tuple3<type> &source, const type W){
      x = source.x;
      y = source.y;
      z = source.z;
      w = W;
    }

    inline operator const type*() const { return &x; }
    inline operator type*() { return &x; }   

	  inline const type  operator[](int i) const { return ((type*)&x)[i]; }
	  inline       type &operator[](int i)       { return ((type*)&x)[i]; }
 
    inline Tuple4 &operator = (const Tuple4<type> &source){
      x = source.x;
      y = source.y;
      z = source.z;
      w = source.w;
      return *this;
    }

    inline Tuple4 &operator = (const Tuple3<type> &source){
      x = source.x;
      y = source.y;
      z = source.z;
      w = 1.0f;
      return *this;
    }

    inline Tuple4 &operator = (const type* source){
      x = source[0];
      y = source[1];
      z = source[2];
      w = source[3];
      return *this;
    }

    inline Tuple4 &operator =( type* source){
      x = source[0];
      y = source[1];
      z = source[2];
      w = source[3];
      return *this;
    }

    inline Tuple4 operator + (const Tuple4<type> &right){
      return Tuple4(right.x + x, right.y + y, right.z + z, right.w + w );
    }

    inline Tuple4 operator - (const Tuple4<type>  &right){
      return Tuple4(-right.x + x, -right.y + y, -right.z + z, -right.w + w );
    }

    inline Tuple4 operator * (const float scale){
      return Tuple4(x*scale, y*scale, z*scale, w*scale);
    }

    inline Tuple4 operator / (const float scale){
      if(scale) return Tuple4(x/scale, y/scale, z/scale, w/scale);
      return Tuple4(0, 0, 0, 0);
    }

    inline void operator += (const Tuple4<type> &right){
      x +=right.x;
      y +=right.y;
      z +=right.z;
      w +=right.w;
    }

    inline void operator -= (const Tuple4<type> &right){
      x-=right.x;
      y-=right.y;
      z-=right.z;
      w-=right.w;
    }

    inline void negate()
    {
      x = -x;
      y = -y;
      z = -z;
      w = -w;
    }

    inline void clamp(const type min, const type max)
    {
      x = x < min ? min : x > max ? max : x;
      y = y < min ? min : y > max ? max : y;
      z = z < min ? min : z > max ? max : z;
      w = w < min ? min : w > max ? max : w;
    }

    inline void operator *= (const type scale){
      x*=scale;
      y*=scale;
      z*=scale;
      w*=scale;
    }

    inline void operator /= (const type scale){
      if(!scale)  return;
      x/=scale;
      y/=scale;
      z/=scale;
      w/=scale;
    }

    inline bool operator == (const Tuple4<type> &right){
      return (x == right.x &&
              y == right.y &&
              z == right.z &&
              w == right.w);
    }

    bool operator != (const Tuple4<type> &right){
      return !(x == right.x &&
               y == right.y &&
               z == right.z &&
               w == right.w);
    }

    inline void set(type xyzw){
      x = 
      y =  
      z =  
      w = xyzw;
    }

    inline void set(type nx, type ny, type nz, type nw){
      x = nx;
      y = ny;
      z = nz;
      w = nw;
    }

    inline void set(const Tuple4<type>  & vec){
      x = vec.x;
      y = vec.y;
      z = vec.z;
      w = vec.w;   
    }

    inline void set(const Tuple3<type>  & vec, const type W){
      x = vec.x;
      y = vec.y;
      z = vec.z;
      w = W;   
    }

    inline void set(const Tuple3<type>  & vec){
      x = vec.x;
      y = vec.y;
      z = vec.z;
      w = type(1.0);   
    }

    friend ostream & operator<< ( ostream & out, const Tuple4<type> & right){
      return out << "Tuple4( " << right.x
                               << ", "
                               << right.y
                               << ", "
                               << right.z
                               << ", "
                               << right.w
                               << ")\n";
    }

    type x, y, z, w;
};

typedef Tuple4<int   > Tuple4i;
typedef Tuple4<float > Tuple4f;
typedef Tuple4<double> Tuple4d;

#endif
