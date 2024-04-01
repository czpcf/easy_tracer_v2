/*
resource group
consisting of shapes, normals, bxdfs, textures...
but it does NOT support intersection test
*/
#ifndef RESOURCE_GROUP_H
#define RESOURCE_GROUP_H

#include "../tensor/vec2.hpp"
#include "../tensor/vec3.hpp"
#include "../tensor/mat3.hpp"
#include "../ray/ray.hpp"
#include "../ray/ray_hit.hpp"

class ResourceGroup {
public:
    ResourceGroup();
    virtual ~ResourceGroup() = default;
    
    /// return number of objects in this group
    virtual int n_objects();

    /// return normal of the n-th shape according to the local coordinate
    virtual Vec3 get_normal_local(int, const Vec2 &);
};
#endif
