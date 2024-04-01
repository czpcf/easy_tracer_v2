#ifndef RESOURCE_MESH_H
#define RESOURCE_MESH_H

#include "resource.hpp"
#include "../geometry/triangle.hpp"
#include "../texture/texture.hpp"

class ResourceTriangle: public Resource {
public:
    ResourceTriangle();
    ResourceTriangle(
        const Vec3 &n1, const Vec3 &n2, const Vec3 &n3,
        const Vec2 &uv1, const Vec2 &uv2, const Vec2 &uv3,
        Sampler *s,
        Bxdf *b,
        Texture *t);
    ~ResourceTriangle() override = default;

    /// return uv coordinate of the n-th shape according to the local coordinate
    UV local_to_uv(const Vec2 &) override;

    /// return normal of the n-th shape according to local & uv
    Vec3 get_color(const Vec2 &, const UV &) override;

    /// return normal of the n-th shape according to local & uv
    Vec3 get_normal(const Vec2 &, const UV &) override;

    /// return bxdf of the n-th shape according to local & uv
    Bxdf *get_bxdf(const Vec2 &, const UV &) override;

    /// return sampler of the n-th shape according to local & uv
    Sampler *get_sampler(const Vec2 &, const UV &) override;

private:
    Vec3 norm1, norm2, norm3;
    Vec2 uv1, uv2, uv3;
    Sampler *sampler;
    Bxdf *bxdf;
    Texture *texture;
};

class ResourceGroupMesh: ResourceGroup {
public:
    ResourceGroupMesh();
    ~ResourceGroupMesh() override = default;
    
    /// @brief number of objects in the group
    void set_size(int) override;
    
    /// @brief return number of objects in this group
    int n_objects() override;

    /// @brief return the pointer of the n-th shape
    Geometry* get_shape(int n) override;

    /// @brief return the resource of the n-th shape
    Resource* get_info(int n) override;

    /// @brief add resource
    void add(const Triangle& t, const ResourceTriangle &info);

private:
    std::vector<Triangle> triangles;
    std::vector<ResourceTriangle> infos;
};
#endif