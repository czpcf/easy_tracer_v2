#include <assert.h>
#include "resource_light_global.hpp"

ResourceLightGlobal::ResourceLightGlobal() {
}

ResourceLightGlobal::ResourceLightGlobal(
    LightGlobal *_light_global,
    Sampler *s,
    Bxdf *b,
    TextureSimple *t,
    const Vec3 &e):
    light_global(_light_global),
    sampler(s),
    bxdf(b),
    texture(t),
    emittor(e) {};

UV ResourceLightGlobal::local_to_uv(const Vec2 &) {
    return UV();
}

Surface ResourceLightGlobal::get_surface(const Vec2 &local) {
    fprintf(stderr, "cannot get surface info of global light !\n");
    assert(0);
    return Surface();
}

Vec3 ResourceLightGlobal::get_emittor() {
    return emittor;
}

Geometry *ResourceLightGlobal::get_shape() {
    return light_global->get_shape();
}

LightGlobal *ResourceLightGlobal::get_light() {
    return light_global;
}





Light *ResourceGroupLightGlobal::get_light(int n) {
    return infos[n].get_light();
}

ResourceGroupLightGlobal::ResourceGroupLightGlobal() {
}

void ResourceGroupLightGlobal::set_size(int n) {
    infos.reserve(n);
}

int ResourceGroupLightGlobal::n_objects() {
    return infos.size();
}

ResourceLight *ResourceGroupLightGlobal::get_info(int n) {
    return &infos[n];
}

void ResourceGroupLightGlobal::add(const ResourceLightGlobal &info) {
    infos.push_back(info);
}

void ResourceGroupLightGlobal::trans(Mat3 &T) {
    for(int i = 0; i < n_objects(); ++i) {
        get_info(i)->get_shape()->trans(T);
    }
}