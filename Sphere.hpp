#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "Surface.hpp"
#include "Ray.hpp"
#include "Vect3.hpp"

#include <cmath>

class Sphere : public Surface
{
  public:

    double radius;
    double radius2;

    Sphere() {};
    Sphere(const Vect3f & c,
          const double & r,
          const Vect3f & sc,
          const double & reflect,
          const double & transp,
          const Vect3f & ec); //:
           
    bool intersect(const Ray & r, double & t, double & t0) const;
    Vect3f normal_hit(Vect3f &phit) const;
};
Sphere::Sphere(const Vect3f & c,
          const double & r,
          const Vect3f & sc,
          const double & reflect,
          const double & transp,
          const Vect3f & ec)
{
  this->radius = r;
  this->radius2 = r*r;
  this->emission_color = ec;
  this->surface_color = sc;
  this->center = c;
  this->reflection = reflect;
  this->transparency = transp;
};
Vect3f Sphere::normal_hit(Vect3f &phit) const
{
  return phit - this->center;
}
bool Sphere::intersect(const Ray & ray, double & t0, double & t1) const
{
    Vect3f L   = center - ray.point;
    double Tca = L.dot(ray.direction);
    double D2  = (L.dot(L) - Tca * Tca);

    if (Tca < 0) return false;
    if (D2 > radius2) return false;

    double Thc = sqrt(radius2 - D2);
    t0         = Tca - Thc;
    t1         = Tca + Thc;
    return true;
};
#endif
