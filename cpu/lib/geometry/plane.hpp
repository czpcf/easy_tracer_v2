/*
plane
*/

#ifndef PLANE_H
#define PLANE_H

#include "geometry.hpp"

class Plane: public Geometry {
public:
    Plane();
    /// @brief origin, z-axis, x-axis, y-axis
    Plane(const Vec3 &, const Vec3 &, const Vec3 &, const Vec3 &);
    ~Plane() override = default;

    /// @brief intersection with the line from point p in direction d
    /// if intersected, return true and set ray-hit
    /// otherwise return false and do nothing
    bool inter(const Ray &, RayHit &) override;

    /// @brief intersection with the line from point p in direction d
    /// if intersected, return true and UPDATE ray-hit
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

    /// @brief return z-axis
    const Vec3 &get_z() const;

private:

    Vec3 origin;
    Vec3 z;
    Vec3 x;
    Vec3 y;
};

#endif