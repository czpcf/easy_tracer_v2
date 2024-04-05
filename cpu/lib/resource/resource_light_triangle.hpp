#ifndef RESOURCE_LIGHT_TRIANGLE_H
#define RESOURCE_LIGHT_TRIANGLE_H

#include "resource.hpp"
#include "emittor.hpp"
#include "../light/light_triangle.hpp"
#include "../texture/texture_simple.hpp"

class ResourceLightTriangle: public ResourceLight {
public:
    ResourceLightTriangle();
    ResourceLightTriangle(
        LightTriangle *_light_triangle,
        Sampler *s,
        Bxdf *b,
        Texture *t,
        const Vec3 &e);
    ~ResourceLightTriangle() override = default;

    /// @brief return uv coordinate of the n-th shape according to the local coordinate
    UV local_to_uv(const Vec2 &) override;

    /// @brief return color of the n-th shape according to local & uv
    Vec3 get_color(const Vec2 &, const UV &) override;

    /// @brief return normal of the n-th shape according to local & uv
    Vec3 get_normal(const Vec2 &, const UV &) override;

    /// @brief return bxdf of the n-th shape according to local & uv
    Bxdf *get_bxdf(const Vec2 &, const UV &) override;

    /// @brief return sampler of the n-th shape according to local & uv
    Sampler *get_sampler(const Vec2 &, const UV &) override;

    /// @brief return emittor
    Vec3 get_emittor() override;

    /// @brief return shape
    Geometry *get_shape() override;

    /// @brief return the light
    LightTriangle *get_light() override;

private:
    LightTriangle *light_triangle;
    Sampler *sampler;
    Bxdf *bxdf;
    Texture *texture;
    Vec3 emittor;
};

class ResourceGroupLightTriangle: public ResourceGroupLight {
public:
    ResourceGroupLightTriangle();
    ~ResourceGroupLightTriangle() override = default;
    
    /// @brief number of objects in the group
    void set_size(int) override;
    
    /// @brief return number of objects in this group
    int n_objects() override;

    /// @brief return the resource of the n-th shape
    ResourceLight *get_info(int n) override;

    /// @brief return the n-th light
    Light *get_light(int n) override;

    /// @brief add resource
    void add(const ResourceLightTriangle &info);
    
    /// @brief apply transform
    void trans(Mat3 &T) override;

private:
    std::vector<ResourceLightTriangle> infos;
};
#endif