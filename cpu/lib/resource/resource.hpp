/*
resource
*/
#ifndef RESOURCE_H
#define RESOURCE_H

#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "../tensor/vec2.hpp"
#include "../tensor/vec3.hpp"
#include "../tensor/mat3.hpp"
#include "../ray/ray.hpp"
#include "../ray/ray_hit.hpp"
#include "../bxdf/bxdf.hpp"
#include "../sampler/sampler.hpp"
#include "../geometry/geometry.hpp"

class Resource {
public:
    Resource();
    virtual ~Resource() = default;

    /// @brief return uv coordinate of the n-th shape according to the local coordinate
    virtual UV local_to_uv(const Vec2 &) = 0;

    /// @brief return surface info
    virtual Surface get_surface(const Vec2 &local) = 0;

    /// @brief return the shape
    virtual Geometry *get_shape() = 0;
};

class ResourceGroup {
public:
    ResourceGroup();
    virtual ~ResourceGroup() = default;
    
    /// @brief number of objects in the group
    virtual void set_size(int n) = 0;
    
    /// @brief return number of objects in this group
    virtual int n_objects() = 0;

    /// @brief return the resource of the n-th shape
    virtual Resource *get_info(int n) = 0;

    /// @brief return the pointer of the n-th shape
    Geometry *get_shape(int n);

    /// @brief apply transform
    virtual void trans(Mat3 &T) = 0;
};
#endif
