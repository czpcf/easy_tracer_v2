/*
vector in 3d
*/
#ifndef VEC3_H
#define VEC3_H

class Vec3 {
public:
    float x;
    float y;
    float z;

    Vec3();
    ~Vec3();
    Vec3(float, float, float);
    Vec3 operator+(const Vec3 &b) const;
    Vec3 operator-(const Vec3 &b) const;
    Vec3 operator-() const;
    Vec3 operator*(const float d) const;
    Vec3 operator/(const float d) const;
    void operator+=(const Vec3 &b);
    void operator-=(const Vec3 &b);
    void operator*=(const float d);
    void operator/=(const float d);
    void operator*=(const Vec3 &b);
    void operator/=(const Vec3 &b);

    /// @brief return the element-wise product: (a.x * b.x, a.y * b.y, a.z * b.z)
    const Vec3 operator*(const Vec3 &b) const;

    /// @brief return the cross product: a x b
    const Vec3 cross(const Vec3 &b) const;

    /// @brief return x + y + z;
    float sum();

    /// @brief return the dot product: <a, b>
    float dot(const Vec3 &b) const;

    /// @brief return the norm of this vector, errors will occur if the vector is 0
    const Vec3 norm() const;

    /// @brief normalize this vector, errors will occur if the vector is 0
    void normalize();

    /// @brief return the length of the vector, errors will occur if the vector is 0
    const float len() const;

    /// @brief return the angle in radians between a and b, safely
    float angle(const Vec3 &b);

    /// @brief return the squared length of the vector
    float square() const;

    /// @brief reflection via b
    Vec3 reflect(const Vec3 &b);

    /// @brief rotate by b with theta radians, counter-clockwise
    Vec3 rotate(const Vec3 &b, float theta);

    /// @brief return (|x|,|y|,|z|)
    Vec3 abs();

    /// @brief return max(x,y,z)
    float max();

    /// @brief return min(x,y,z)
    float min();

    /// @brief return (1 - t) * a + t * b
    static Vec3 lerp(const Vec3 &a, const Vec3 &b, float t);

    /// @brief if any value is nan or inf
    const bool have_bad() const;
};

/// @brief return a x b if a and b is not on a line,
/// otherwise return c that <a,c> = 0
Vec3 cross_safe(Vec3 a, Vec3 b);

/// @brief element-wise min
Vec3 min(Vec3 a, Vec3 b);

/// @brief element-wise max
Vec3 max(Vec3 a, Vec3 b);
#endif
