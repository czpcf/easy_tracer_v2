#include <assert.h>
#include "resource_light_point.hpp"

ResourceLightPoint::ResourceLightPoint() {
}

ResourceLightPoint::ResourceLightPoint(
    LightPoint *_light_point,
    Sampler *s,
    Bxdf *b,
    TextureSimple *t,
    const Vec3 &e):
    light_point(_light_point),
    sampler(s),
    bxdf(b),
    texture(t),
    emittor(e) {};

UV ResourceLightPoint::local_to_uv(const Vec2 &) {
    return UV();
}

Vec3 ResourceLightPoint::get_color(const Vec2 &, const UV &) {
    return texture->get(UV());
}

Vec3 ResourceLightPoint::get_normal(const Vec2 &, const UV &) {
    fprintf(stderr, "cannot compute normal on point light!\n");
    assert(0);
    return Vec3();
}

Bxdf *ResourceLightPoint::get_bxdf(const Vec2 &, const UV &) {
    fprintf(stderr, "cannot get bxdf on point light!\n");
    assert(0);
    return nullptr;
}

Sampler *ResourceLightPoint::get_sampler(const Vec2 &, const UV &) {
    fprintf(stderr, "cannot get sampler on point light!\n");
    assert(0);
    return nullptr;
}

Vec3 ResourceLightPoint::get_emittor() {
    return emittor;
}

Geometry *ResourceLightPoint::get_shape() {
    return light_point->get_shape();
}

LightPoint *ResourceLightPoint::get_light() {
    return light_point;
}





Light *ResourceGroupLightPoint::get_light(int n) {
    return infos[n].get_light();
}

ResourceGroupLightPoint::ResourceGroupLightPoint() {
}

void ResourceGroupLightPoint::set_size(int n) {
    infos.reserve(n);
}

int ResourceGroupLightPoint::n_objects() {
    return infos.size();
}

ResourceLight *ResourceGroupLightPoint::get_info(int n) {
    return &infos[n];
}

void ResourceGroupLightPoint::add(const ResourceLightPoint &info) {
    infos.push_back(info);
}

void ResourceGroupLightPoint::trans(Mat3 &T) {
    for(int i = 0; i < n_objects(); ++i) {
        get_info(i)->get_shape()->trans(T);
    }
}