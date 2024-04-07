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

Surface ResourceLightPoint::get_surface(const Vec2 &local) {
    fprintf(stderr, "cannot get surface info of light point !\n");
    assert(0);
    return Surface();
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