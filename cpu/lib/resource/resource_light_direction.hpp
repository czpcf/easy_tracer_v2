#ifndef RESOURCE_LIGHT_DIRECTIONAL_H
#define RESOURCE_LIGHT_DIRECTIONAL_H

#include "resource.hpp"
#include "emittor.hpp"
#include "../light/light_direction.hpp"
#include "../texture/texture_simple.hpp"

class ResourceLightDirection: public ResourceLight {
public:
    ResourceLightDirection();
    ResourceLightDirection(
        LightDirection *_light_direction,
        Sampler *s,
        Bxdf *b,
        TextureSimple *t,
        const Vec3 &e);
    ~ResourceLightDirection() override = default;

    /// @brief return uv coordinate of the n-th shape according to the local coordinate
    UV local_to_uv(const Vec2 &) override;

    /// @brief return surface info
    Surface get_surface(const Vec2 &local) override;

    /// @brief return emittor
    Vec3 get_emittor() override;

    /// @brief return shape
    Geometry *get_shape() override;

    /// @brief return the light
    LightDirection *get_light() override;

private:
    LightDirection *light_direction;
    Sampler *sampler;
    Bxdf *bxdf;
    Texture *texture;
    Vec3 emittor;
};

class ResourceGroupLightDirection: public ResourceGroupLight {
public:
    ResourceGroupLightDirection();
    ~ResourceGroupLightDirection() override = default;
    
    /// @brief number of objects in the group
    void set_size(int) override;
    
    /// @brief return number of objects in this group
    int n_objects() override;

    /// @brief return the resource of the n-th shape
    ResourceLight *get_info(int n) override;

    /// @brief return the n-th light
    Light *get_light(int n) override;

    /// @brief add resource
    void add(const ResourceLightDirection &info);
    
    /// @brief apply transform
    void trans(Mat3 &T) override;

private:
    std::vector<ResourceLightDirection> infos;
};
#endif