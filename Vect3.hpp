#ifndef VECT3_HPP
#define  VECT3_HPP
#include <iostream> 
#include <cmath>

template<typename T>
class Vect3
{
    public:
        T x,y,z;
         // Constructor
        Vect3() : x(0), y(0), z(0) {};
        Vect3(T i) : x(i), y(i), z(i) {};
        Vect3(T x,T y,T z) : x(x), y(y), z(z) {};
        //Functions
        Vect3<T> operator*(const T &f) const { return Vect3<T>(x*f, y*f,z*f);}
        Vect3<T> operator/(const T &f) const { return Vect3<T>(x/f, y/f,z/f);}
        Vect3<T> operator*(const Vect3<T> &v3) const { return Vect3<T>(x*v3.x, y*v3.y,z*v3.z);}
        T dot(const Vect3<T> &v3) const { return x * v3.x + y * v3.y + z * v3.z; }
        Vect3<T> operator+(const Vect3<T> &v3) const { return Vect3<T>(x + v3.x, y + v3.y, z + v3.z); }
        Vect3<T> operator-(const Vect3<T> &v3) const { return Vect3<T>(x - v3.x, y - v3.y, z - v3.z); }
        //Vect3<T> &operator+=(const Vect3<T> &v3) const { (x += v3.x, y += v3.y, z += v3.z); return *this; }
        Vect3<T>& operator += (const Vect3<T> &v) { x += v.x, y += v.y, z += v.z; return *this; } 
        Vect3<T> &operator*=(const Vect3<T> &v3) const { (x *= v3.x, y *= v3.y, z *= v3.z); return *this; }
        Vect3<T> operator-() const {return Vect3<T>(-x,-y,-z);}
        T length2() const { return x*x +y*y + z*z;}
        T length() const { return std::sqrt(length2());}
        friend std::ostream & operator << (std::ostream &os, const Vect3<T> &v) 
        { 
            os << "[" << v.x << " " << v.y << " " << v.z << "]"; 
            return os; 
        } 
        Vect3 & normalize()
        {
            T nor2 = length2(); 
            if (nor2 > 0) { 
                T invNor = 1 / sqrt(nor2); 
                x *= invNor, y *= invNor, z *= invNor; 
            } 
            return *this; 
        };
};


typedef Vect3<double> Vect3f; 
#endif