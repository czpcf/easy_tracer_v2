#ifndef RESOURCE_LIGHT_GLOBAL_H
#define RESOURCE_LIGHT_GLOBAL_H

#include "resource.hpp"
#include "emittor.hpp"
#include "../light/light_global.hpp"
#include "../texture/texture_simple.hpp"

class ResourceLightGlobal: public ResourceLight {
public:
    ResourceLightGlobal();
    ResourceLightGlobal(
        LightGlobal *_light_global,
        Sampler *s,
        Bxdf *b,
        TextureSimple *t,
        const Vec3 &e);
    ~ResourceLightGlobal() override = default;

    /// @brief return uv coordinate of the n-th shape according to the local coordinate
    UV local_to_uv(const Vec2 &) override;

    /// @brief return surface info
    Surface get_surface(const Vec2 &local) override;

    /// @brief return emittor
    Vec3 get_emittor() override;

    /// @brief return shape
    Geometry *get_shape() override;

    /// @brief return the light
    LightGlobal *get_light() override;

private:
    LightGlobal *light_global;
    Sampler *sampler;
    Bxdf *bxdf;
    Texture *texture;
    Vec3 emittor;
};

class ResourceGroupLightGlobal: public ResourceGroupLight {
public:
    ResourceGroupLightGlobal();
    ~ResourceGroupLightGlobal() override = default;
    
    /// @brief number of objects in the group
    void set_size(int) override;
    
    /// @brief return number of objects in this group
    int n_objects() override;

    /// @brief return the resource of the n-th shape
    ResourceLight *get_info(int n) override;

    /// @brief return the n-th light
    Light *get_light(int n) override;

    /// @brief add resource
    void add(const ResourceLightGlobal &info);
    
    /// @brief apply transform
    void trans(Mat3 &T) override;

private:
    std::vector<ResourceLightGlobal> infos;
};
#endif