#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "Surface.hpp"
#include "Ray.hpp"
#include "Vect3.hpp"

#include <cmath>

class Triangle : public Surface
{
  public:

    double radius;
    double radius2;

    Triangle() {};
    Triangle(const Vect3f & c,
          const double & r,
          const Vect3f & sc,
          const double & reflect,
          const double & transp,
          const Vect3f & ec); //:
           
    bool intersect(const Ray & r, double & t, double & t0) const;
    Vect3f normal_hit(Vect3f &phit) const;
};
#endif