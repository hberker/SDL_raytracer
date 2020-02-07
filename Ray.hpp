#ifndef RAY_HPP
#define  RAY_HPP

#include "Vect3.hpp"

class Ray
{
    public:
    
        Vect3f point;
        Vect3f direction;
        // Constructor
        Ray() : point(Vect3f()), direction(Vect3f()){};
        Ray(Vect3f p, Vect3f dir) : point(p), direction(dir) {};
};
#endif