#ifndef PLANE_HPP
#define PLANE_HPP

#include "Surface.hpp"
#include "Ray.hpp"
#include "Vect3.hpp"
#include <iostream>

#include <cmath>

class Plane : public Surface
{
    public:
        Vect3f normal;

        Plane(const Vect3f & normal,
            const Vect3f & center,
            const Vect3f & sc,
            const double & reflect,
            const double & transp,
            const Vect3f & ec); //:
            
        bool intersect(const Ray & r, double & t, double & t0) const;
        Vect3f normal_hit(Vect3f &phit) const;
};

Plane::Plane(const Vect3f & normal,
        const Vect3f & center,
          const Vect3f & sc,
          const double & reflect,
          const double & transp,
          const Vect3f & ec)
{
  this->center = center;
  this->normal = normal;
  this->emission_color = ec;
  this->surface_color = sc;
  this->reflection = reflect;
  this->transparency = transp;
};
Vect3f Plane::normal_hit(Vect3f &phit) const
{
    return this->normal;
}

bool Plane::intersect(const Ray & r, double & t, double & t0) const
{
    double denominator = r.direction.dot(normal);
    if(std::abs(denominator) > 0.0001)
    {
        Vect3f diff =this->center - r.point;
        t = normal.dot(diff) / denominator;

        if( t >  0.0001 )
        {
            return true;
        }
    }
    return false;

}
#endif