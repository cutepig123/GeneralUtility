#ifndef DISTANCE_OBJECT_H
#define DISTANCE_OBJECT_H

#include "../Tools/Logger.h"

template <class T>
class DistanceObject
{
  public:
    DistanceObject()
    {
      distance = 0.0f;
    }

    DistanceObject(T objectArg)
    {
      distance = 0.0f;
      object   = objectArg;
    }

    DistanceObject(const DistanceObject &copy)
    {
      operator=(copy);
    }

    DistanceObject &operator =(const DistanceObject &copy)
    {
      if(this != &copy)
      {
        distance = copy.distance;
        object   = copy.object;
      }
      return *this;
    }

    bool operator ==(const DistanceObject &copy)
    {
       return (distance == copy.distance);
    }

    bool operator >=(const DistanceObject &copy)
    {
       return (distance >= copy.distance);
    }

    bool operator <=(const DistanceObject &copy)
    {
       return (distance <= copy.distance);
    }

    bool operator >(const DistanceObject &copy)
    {
       return (distance > copy.distance);
    }
 
    bool operator < (const DistanceObject &copy)
    {
       return (distance < copy.distance);
    }
   
    void setDistance(float dist)
    {
      distance = dist;
    }

    float getDistance()
    {
     return distance;
    }

    void setObject(T objectArg)
    {
      object =objectArg;
    }

    T &getObject()
    {
      return object;
    }
   private:
     float distance;
     T     object;

};

#endif
