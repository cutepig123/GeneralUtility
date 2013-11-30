#ifndef PERLIN_H
#define PERLIN_H
 
#include "../Tools/Logger.h"

class Perlin
{
  private:
    static void   normalize2(double v[2]);
    static void   normalize3(double v[3]);
  
  public:
    static void   setNoiseFrequency(int frequency);
    static int    getNoiseFrequency();

    static double noise1(double arg);
    static double noise2(double vec[2]);
    static double noise3(double vec[3]);
    static void   initialize();
    static double noise1D(double x,double alpha,double beta,int n);
    static double noise2D(double x, double y, double alpha, double beta, int n);
    static double noise3D(double x, double y, double z, double alpha, double beta, int n);
};

#endif
