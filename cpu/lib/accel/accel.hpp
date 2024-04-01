/*
accel
*/

#ifndef ACCEL_H
#define ACCEL_H

#include <vector>
#include "../tensor/mat3.hpp"
#include "../tensor/vec2.hpp"
#include "../tensor/vec3.hpp"
#include "../geometry/geometry.hpp"
#include "../resource/resource.hpp"
#include "../ray/ray.hpp"
#include "../ray/ray_hit.hpp"
#include "box.hpp"

class Accel {
public:
    Accel();
    virtual ~Accel() = default;

    /// @brief set size of elements
    virtual void set_size(int n) = 0;

    /// @brief add single resource
    virtual void add(const Geometry*, const Resource*) = 0;

    /// @brief add resource group
    virtual void add(const ResourceGroup &) = 0;

    /// @brief intersection with the line from point p in direction d
    /// if intersected, return true and set ray-hit
    /// otherwise return false and do nothing
    virtual bool inter(Ray, RayHit &) = 0;

    /// @brief intersection with the line from point p in direction d
    /// if intersected, return true and UPDATE ray-hit
    /// otherwise return false and do nothing
    virtual bool inter_update(Ray, RayHit &) = 0;
    
    /// @brief check if the ray hits the shape given the distance,
    /// distance should be squared
    virtual bool if_inter_dis(Ray, float) = 0;

private:
    std::vector<Box> boxes;
    std::vector<Geometry*> shapes;
    std::vector<Resource*> infos;
};
#endif