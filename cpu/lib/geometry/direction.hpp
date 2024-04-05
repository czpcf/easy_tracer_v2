/*
direction
only suitable for directional light
*/

#ifndef DIRECTION_H
#define DIRECTION_H

#include "geometry.hpp"

class Direction: public Geometry {
public:
    Direction();
    Direction(const Vec3 &);
    ~Direction() override = default;

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

    /// @brief return d
    const Vec3 get_d() const;

protected:
    Vec3 d;
};

#endif