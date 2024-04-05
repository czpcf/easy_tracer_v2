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
    virtual void add(Geometry *) = 0;

    /// @brief add resource group
    virtual void add(ResourceGroup *) = 0;

    /// @brief intersection with the line from point p in direction d
    /// if intersected, return true and set ray-hit
    /// otherwise return false and do nothing
    virtual bool inter(const Ray&, RayHit &) = 0;

    /// @brief check if the ray hits the shape given the distance,
    /// distance should be squared
    virtual bool if_inter_dis(const Ray&, float) = 0;

    /// @brief check if the ray hits the shape given the id,
    /// distance should be squared
    virtual bool if_inter_id(const Ray&, int) = 0;

    /// @brief build accelerator
    virtual void build() = 0;

protected:
    std::vector<Box> boxes;
    std::vector<Geometry*> shapes;
};
#endif