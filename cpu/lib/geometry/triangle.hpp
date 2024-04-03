/*
triangle
*/
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "geometry.hpp"
#include "../tensor/vec3.hpp"
#include "../tensor/mat3.hpp"

class Triangle: public Geometry {
public:
    Triangle();
    /// @brief counter-clockwise: p1 -> p2 -> p3
    Triangle(Vec3, Vec3, Vec3);
    
    /// @brief intersection with the line from point p in direction d
    /// if intersected, return true and set ray-hit
    /// otherwise return false and do nothing
    bool inter(const Ray &, RayHit &) override;

    /// @brief intersection with the line from point p in direction d
    /// if intersected, return true and set ray-hit
    /// otherwise return false and do nothing
    bool inter_update(const Ray &, RayHit &) override;
    
    /// @brief check if the ray hits the shape given the distance,
    /// distance should be squared
    bool if_inter_dis(const Ray &, float) override;

    /// @brief apply the transform to the geometry permanently
    void trans(Mat3& T) override;

    /// @brief debug using fprintf
    void debug() override;

    /// @brief return bounding box
    Box bound() override;

    /// @brief return area
    float area() override;

    /// @brief return norm
    const Vec3 get_norm() const;

private:
    /// @brief fast pre-calculation
    unsigned short type;
    /// @brief area
    float _area;
    /// @brief 3 vertices of the triangle
    Vec3 p1, p2, p3;
    /// @brief normal of the triangle
    Vec3 norm;
};
#endif