/*
sphere
*/
#ifndef SPHERE_H
#define SPHERE_H

#include "geometry.hpp"
#include "../tensor/vec3.hpp"
#include "../tensor/mat3.hpp"

class Sphere: public Geometry {
public:
    Sphere();
    ~Sphere();

    /// @brief z-axis, x-axis, origin and the radius
    Sphere(Vec3, Vec3, Vec3, float);

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
    
    /// @brief convert intersecion into local
    Vec2 inter_to_local(const Vec3 &) override;

    /// @brief apply the transform to the geometry permanently
    void trans(Mat3& T) override;

    /// @brief debug using fprintf
    void debug() override;

    /// @brief return bounding box
    Box bound() override;

    /// @brief return area
    float area() override;

    const Vec3 &z_axis() const;
    const Vec3 &y_axis() const;
    const Vec3 &x_axis() const;
    
private:
    /// @brief area
    float _area;
    /// @brief radius
    float r;
    /// @brief z-axis of the sphere. Use [0, pi]x[0, 2pi] -> S^2 mapping
    /// (sin\theta cos\phi, sin\theta sin\phi, cos\theta)
    Vec3 z;
    /// @brief x-axis of the sphere, and then y-axis is determined by z * x
    Vec3 x, y;
    /// @brief origin of the sphere
    Vec3 ori;
};
#endif