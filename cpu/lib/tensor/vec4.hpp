/*
vector in 4d
*/
#ifndef VEC4_H
#define VEC4_H
#include "vec3.hpp"

class Vec4 {
public:
    float x;
    float y;
    float z;
    float w;
    Vec4();
    ~Vec4();
    Vec4(float, float, float, float);
    Vec4 operator+(const Vec4 &b) const;
    Vec4 operator-(const Vec4 &b) const;
    Vec4 operator-() const;
    Vec4 operator*(const float d) const;
    Vec4 operator/(const float d) const;
    void operator+=(const Vec4 &b);
    void operator-=(const Vec4 &b);
    void operator*=(const float d);
    void operator/=(const float d);
    void operator*=(const Vec4 &b);
    void operator/=(const Vec4 &b);

    /// @brief return the element-wise product: (a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w)
    Vec4 operator*(const Vec4 &b);

    /// @brief return x + y + z + w;
    float sum();

    /// @brief return the dot product: <a, b>
    float dot(const Vec4 &b);

    /// @brief return the norm of this vector, errors will occur if the vector is 0
    Vec4 norm();

    /// @brief normalize this vector, errors will occur if the vector is 0
    void normalize();

    /// @brief return the length of the vector, errors will occur if the vector is 0
    float len();

    /// @brief return the angle in radians between a and b, safely
    float angle(const Vec4 &b);

    /// @brief return the squared length of the vector
    float square();

    /// @brief return (x,y,z)
    Vec3 xyz();
};

#endif