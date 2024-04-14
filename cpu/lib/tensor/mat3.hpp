/*
represents f(x) = Ax + b from R^3 to R^3,
where A is orthogonal,
x, y, z axis must be scaled with the same value
*/
#ifndef MAT3_H
#define MAT3_H

#include "vec3.hpp"
class Mat3 {
public:

    /// @brief the representation of the transform f(x) = Ax + b,
    /// where A is a 3*3 matrix and b is a 3d vector
    Mat3();
    ~Mat3();
    
    /// @brief return the transform which represents a shift: 0 -> p
    Mat3(Vec3 p);
    
    /// @brief return the transform which represents a counter-clockwise
    /// rotation around the vector n with theta radians
    Mat3(Vec3 n, float theta);

    /// @brief the representation of the transform f(x) = Ax + b,
    /// where A is a 3*3 matrix and b is a 3d vector
    Mat3(float array_a[9], float array_b[3]);

    /// @brief the representation of the transform f(x) = Ax + b,
    /// where A is a 3*3 matrix and b is a 3d vector
    Mat3(float matrix_a[3][3], float array_b[3]);
    
    /// @brief return f(g(x))
    Mat3 operator*(const Mat3&g);

    /// @brief return f(x) = I * x + 0
    static Mat3 id();

    /// @brief return f(x)=Ax+b
    Vec3 map(const Vec3&x);

    /// @brief return Ax
    Vec3 map_scale(const Vec3&x);

    /// @brief return f(x) = x + a
    Vec3 map_bias(const Vec3&x);

    /// @brief return det A
    float det();
    
    /// @brief scaling
    static Mat3 mat_from_scale(Vec3 scale);

    /// @brief transition
    static Mat3 mat_from_trans(Vec3 trans);

    /// @brief ratation
    static Mat3 mat_from_rotate(const Vec3 &b, float theta);

    /// @brief return the look-at matrix
    static Mat3 look_at(const Vec3 &eye, const Vec3 &center, const Vec3 &up);

    /// @brief return the inversion of look-at matrix
    static Mat3 look_at_inv(const Vec3 &eye, const Vec3 &center, const Vec3 &up);

    void debug();

private:
    /// the vector b
    float b[3];

    /// entris of matrix A
    /// very ugly representation... for the sake of speed
    float a[9];
};

#endif