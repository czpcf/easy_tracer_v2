#ifndef RESOURCE_MESH_H
#define RESOURCE_MESH_H

#include "resource.hpp"
#include "../geometry/triangle.hpp"
#include "../texture/texture.hpp"

class ResourceTriangle: public Resource {
public:
    ResourceTriangle();
    ResourceTriangle(
        Triangle *_triangle,
        const Vec3 &n1, const Vec3 &n2, const Vec3 &n3,
        const Vec2 &uv1, const Vec2 &uv2, const Vec2 &uv3,
        Sampler *s,
        Bxdf *b,
        Texture *t);
    ~ResourceTriangle() override = default;

    /// return uv coordinate of the n-th shape according to the local coordinate
    UV local_to_uv(const Vec2 &) override;

    /// return color of the n-th shape according to local & uv
    Vec3 get_color(const Vec2 &, const UV &) override;

    /// return normal of the n-th shape according to local & uv
    Vec3 get_normal(const Vec2 &, const UV &) override;

    /// return bxdf of the n-th shape according to local & uv
    Bxdf *get_bxdf(const Vec2 &, const UV &) override;

    /// return sampler of the n-th shape according to local & uv
    Sampler *get_sampler(const Vec2 &, const UV &) override;

    Geometry *get_shape() override;

private:
    Triangle *triangle;
    Vec3 norm1, norm2, norm3;
    Vec2 uv1, uv2, uv3;
    Sampler *sampler;
    Bxdf *bxdf;
    Texture *texture;
};

class ResourceGroupMesh: public ResourceGroup {
public:
    ResourceGroupMesh();
    /// @brief mesh from obj
    ResourceGroupMesh(const char *filename, Sampler *sampler, Bxdf *bxdf, Texture *texture);
    ~ResourceGroupMesh() override = default;
    
    /// @brief number of objects in the group
    void set_size(int) override;
    
    /// @brief return number of objects in this group
    int n_objects() override;

    /// @brief return the resource of the n-th shape
    Resource* get_info(int n) override;

    /// @brief add resource
    void add(const ResourceTriangle &info);
    
    /// @brief apply transform
    void trans(Mat3 &T) override;

private:
    std::vector<ResourceTriangle> infos;
};
#endif