/*
vector in 2d
*/
#ifndef VEC2_H
#define VEC2_H

class Vec2 {
public:
    float x;
    float y;

    Vec2();
    ~Vec2();
    Vec2(float, float);
    Vec2 operator+(const Vec2 &b) const;
    Vec2 operator-(const Vec2 &b) const;
    Vec2 operator-() const;
    Vec2 operator*(const float d) const;
    Vec2 operator/(const float d) const;
    void operator+=(const Vec2 &b);
    void operator-=(const Vec2 &b);
    void operator*=(const float d);
    void operator/=(const float d);
    void operator*=(const Vec2 &b);
    void operator/=(const Vec2 &b);

    /// @brief return the element-wise product: (a.x * b.x, a.y * b.y)
    Vec2 operator*(const Vec2 &b);

    /// @brief return x + y + z;
    float sum();

    /// @brief
    float dot(const Vec2 &b);

    /// @brief return the norm of this vector, errors will occur if the vector is 0
    Vec2 norm();

    /// @brief normalize this vector, errors may occur if the vector is 0
    void normalize();

    /// @brief return the length of the vector, errors will occur if the vector is 0
    float len();

    /// @brief return the angle in radians between a and b, safely
    float angle(const Vec2 &b);

    /// @brief return the squared length of the vector
    float square();
};

#endif
