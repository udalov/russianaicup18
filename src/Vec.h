#pragma once

#include <cmath>
#include <string>

constexpr inline double sqr(double x) { return x * x; }

struct Vec {
    double x, y, z;

    constexpr Vec() : x(), y(), z() {}
    constexpr Vec(double x, double y, double z) : x(x), y(y), z(z) {}

    constexpr Vec operator+(const Vec& other) const { return Vec(x + other.x, y + other.y, z + other.z); }
    constexpr Vec operator-(const Vec& other) const { return Vec(x - other.x, y - other.y, z - other.z); }
    constexpr Vec operator*(double coeff) const { return Vec(x * coeff, y * coeff, z * coeff); }

    constexpr void operator+=(const Vec& other) { x += other.x; y += other.y; z += other.z; }
    constexpr void operator-=(const Vec& other) { x -= other.x; y -= other.y; z -= other.z; }
    constexpr void operator*=(double coeff) { x *= coeff; y *= coeff; z *= coeff; }

    Vec normalize() const { return *this * (1 / len()); }

    constexpr double dot(const Vec& v) const { return x*v.x + y*v.y + z*v.z; }

    constexpr double sqrLen() const { return x*x + y*y + z*z; }
    double len() const { return sqrt(sqrLen()); }

    constexpr double sqrDist(const Vec& other) const { return sqr(x-other.x) + sqr(y-other.y) + sqr(z-other.z); }
    double distance(const Vec& other) const { return sqrt(sqrDist(other)); }

    void clamp(double length);

    std::string toString() const;
};
