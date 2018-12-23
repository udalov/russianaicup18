#pragma once

#include <cmath>
#include <string>

inline double sqr(double x) { return x * x; }

struct Vec {
    double x, y, z;

    Vec() : x(), y(), z() {}
    Vec(double x, double y, double z) : x(x), y(y), z(z) {}

    Vec operator+(const Vec& other) const { return Vec(x + other.x, y + other.y, z + other.z); }
    Vec operator-(const Vec& other) const { return Vec(x - other.x, y - other.y, z - other.z); }
    Vec operator*(double coeff) const { return Vec(x * coeff, y * coeff, z * coeff); }

    void operator+=(const Vec& other) { x += other.x; y += other.y; z += other.z; }
    void operator-=(const Vec& other) { x -= other.x; y -= other.y; z -= other.z; }
    void operator*=(double coeff) { x *= coeff; y *= coeff; z *= coeff; }

    Vec normalize() const { return *this * (1 / len()); }

    double dot(const Vec& v) const { return x*v.x + y*v.y + z*v.z; }

    double sqrLen() const { return x*x + y*y + z*z; }
    double len() const { return sqrt(sqrLen()); }

    double sqrDist(const Vec& other) const { return sqr(x-other.x) + sqr(y-other.y) + sqr(z-other.z); }
    double distance(const Vec& other) const { return sqrt(sqrDist(other)); }

    void clamp(double length);

    std::string toString() const;
};
