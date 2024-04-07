#ifndef RESOURCE_LIGHT_POINT_H
#define RESOURCE_LIGHT_POINT_H

#include "resource.hpp"
#include "emittor.hpp"
#include "../light/light_point.hpp"
#include "../texture/texture_simple.hpp"

class ResourceLightPoint: public ResourceLight {
public:
    ResourceLightPoint();
    ResourceLightPoint(
        LightPoint *_light_point,
        Sampler *s,
        Bxdf *b,
        TextureSimple *t,
        const Vec3 &e);
    ~ResourceLightPoint() override = default;

    /// @brief return uv coordinate of the n-th shape according to the local coordinate
    UV local_to_uv(const Vec2 &) override;

    /// @brief return surface info
    Surface get_surface(const Vec2 &local) override;

    /// @brief return emittor
    Vec3 get_emittor() override;

    /// @brief return shape
    Geometry *get_shape() override;

    /// @brief return the light
    LightPoint *get_light() override;

private:
    LightPoint *light_point;
    Sampler *sampler;
    Bxdf *bxdf;
    Texture *texture;
    Vec3 emittor;
};

class ResourceGroupLightPoint: public ResourceGroupLight {
public:
    ResourceGroupLightPoint();
    ~ResourceGroupLightPoint() override = default;
    
    /// @brief number of objects in the group
    void set_size(int) override;
    
    /// @brief return number of objects in this group
    int n_objects() override;

    /// @brief return the resource of the n-th shape
    ResourceLight *get_info(int n) override;

    /// @brief return the n-th light
    Light *get_light(int n) override;

    /// @brief add resource
    void add(const ResourceLightPoint &info);
    
    /// @brief apply transform
    void trans(Mat3 &T) override;

private:
    std::vector<ResourceLightPoint> infos;
};
#endif