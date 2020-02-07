#ifndef SURFACE_HPP
#define SURFACE_HPP

#include "Ray.hpp"
#include "Vect3.hpp"

class Surface
{
  public:
    Vect3f center;
    Vect3f surface_color;
    Vect3f emission_color;

    double transparency;
    double reflection;
    
    Surface(){};
  
    virtual Vect3f normal_hit(Vect3f &phit) const  = 0;
    virtual bool intersect(const Ray & r, double & t, double & t0) const = 0;
};



#endif
