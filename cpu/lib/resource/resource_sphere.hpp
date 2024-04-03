#ifndef RESOURCE_SPHERE_H
#define RESOURCE_SPHERE_H

#include "resource.hpp"
#include "../geometry/sphere.hpp"
#include "../texture/texture.hpp"

class ResourceSphere: public Resource {
public:
    ResourceSphere();
    ResourceSphere(
        Sphere *_sphere,
        Sampler *s,
        Bxdf *b,
        Texture *t);
    ~ResourceSphere() override = default;

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

    /// @brief return the shape
    Geometry *get_shape() override;

private:
    Sphere *sphere;
    Sampler *sampler;
    Bxdf *bxdf;
    Texture *texture;
};

class ResourceGroupSphere: public ResourceGroup {
public:
    ResourceGroupSphere();
    ~ResourceGroupSphere() override = default;
    
    /// @brief number of objects in the group
    void set_size(int) override;
    
    /// @brief return number of objects in this group
    int n_objects() override;

    /// @brief return the resource of the n-th shape
    Resource* get_info(int n) override;

    /// @brief add resource
    void add(const ResourceSphere &info);

    /// @brief apply transform
    void trans(Mat3 &T) override;

private:
    std::vector<ResourceSphere> infos;
};
#endif