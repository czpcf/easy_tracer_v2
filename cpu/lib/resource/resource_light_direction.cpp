#include <assert.h>
#include "resource_light_direction.hpp"

ResourceLightDirection::ResourceLightDirection() {
}

ResourceLightDirection::ResourceLightDirection(
    LightDirection *_light_direction,
    Sampler *s,
    Bxdf *b,
    TextureSimple *t,
    const Vec3 &e):
    light_direction(_light_direction),
    sampler(s),
    bxdf(b),
    texture(t),
    emittor(e) {};

UV ResourceLightDirection::local_to_uv(const Vec2 &) {
    return UV();
}

Vec3 ResourceLightDirection::get_color(const Vec2 &, const UV &) {
    return texture->get(UV());
}

Vec3 ResourceLightDirection::get_normal(const Vec2 &, const UV &) {
    fprintf(stderr, "cannot compute normal on directional light!\n");
    assert(0);
    return Vec3();
}

Bxdf *ResourceLightDirection::get_bxdf(const Vec2 &, const UV &) {
    fprintf(stderr, "cannot get bxdf on directional light!\n");
    assert(0);
    return nullptr;
}

Sampler *ResourceLightDirection::get_sampler(const Vec2 &, const UV &) {
    fprintf(stderr, "cannot get sampler on directional light!\n");
    assert(0);
    return nullptr;
}

Vec3 ResourceLightDirection::get_emittor() {
    return emittor;
}

Geometry *ResourceLightDirection::get_shape() {
    return light_direction->get_shape();
}

LightDirection *ResourceLightDirection::get_light() {
    return light_direction;
}




Light *ResourceGroupLightDirection::get_light(int n) {
    return infos[n].get_light();
}

ResourceGroupLightDirection::ResourceGroupLightDirection() {
}

void ResourceGroupLightDirection::set_size(int n) {
    infos.reserve(n);
}

int ResourceGroupLightDirection::n_objects() {
    return infos.size();
}

ResourceLight *ResourceGroupLightDirection::get_info(int n) {
    return &infos[n];
}

void ResourceGroupLightDirection::add(const ResourceLightDirection &info) {
    infos.push_back(info);
}

void ResourceGroupLightDirection::trans(Mat3 &T) {
    for(int i = 0; i < n_objects(); ++i) {
        get_info(i)->get_shape()->trans(T);
    }
}