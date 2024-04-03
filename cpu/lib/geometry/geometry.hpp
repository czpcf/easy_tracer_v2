/*
geometry
its just geometry, do not have normal, bxdf or texture
*/
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <utility>
#include "../tensor/vec2.hpp"
#include "../tensor/vec3.hpp"
#include "../tensor/mat3.hpp"
#include "../accel/box.hpp"
#include "../ray/ray.hpp"
#include "../ray/ray_hit.hpp"
#include "../random/random.hpp"

class Geometry {
public:
    Geometry();
    virtual ~Geometry() = default;

    /// @brief intersection with the line from point p in direction d
    /// if intersected, return true and set ray-hit
    /// otherwise return false and do nothing
    virtual bool inter(const Ray &, RayHit &);

    /// @brief intersection with the line from point p in direction d
    /// if intersected, return true and UPDATE ray-hit
    /// otherwise return false and do nothing
    virtual bool inter_update(const Ray &, RayHit &);
    
    /// @brief check if the ray hits the shape given the distance,
    /// distance should be squared
    virtual bool if_inter_dis(const Ray &, float);

    /// @brief apply the transform to the geometry permanently
    virtual void trans(Mat3& T);

    /// @brief debug using fprintf
    virtual void debug();

    /// @brief return bounding box
    virtual Box bound();

    /// @brief return area
    virtual float area();
};
#endif