#include <assert.h>
#include "resource_light_triangle.hpp"

ResourceLightTriangle::ResourceLightTriangle() {
}

ResourceLightTriangle::ResourceLightTriangle(
    LightTriangle *_light_triangle,
    Sampler *s,
    Bxdf *b,
    Texture *t,
    const Vec3 &e):
    light_triangle(_light_triangle),
    sampler(s),
    bxdf(b),
    texture(t),
    emittor(e) {};

UV ResourceLightTriangle::local_to_uv(const Vec2 &) {
    return UV();
}

Surface ResourceLightTriangle::get_surface(const Vec2 &local) {
    Vec3 norm = light_triangle->get_norm();
    Vec3 x = (light_triangle->get_p2() - light_triangle->get_p1()).norm();
    Vec3 y = norm.cross(x);
    UV uv = local_to_uv(local);
    return Surface(
        norm,
        x,
        y,
        texture->get(uv),
        bxdf,
        sampler
    );
}

Vec3 ResourceLightTriangle::get_emittor() {
    return emittor;
}

Geometry *ResourceLightTriangle::get_shape() {
    return light_triangle->get_shape();
}

LightTriangle *ResourceLightTriangle::get_light() {
    return light_triangle;
}





Light *ResourceGroupLightTriangle::get_light(int n) {
    return infos[n].get_light();
}

ResourceGroupLightTriangle::ResourceGroupLightTriangle() {
}

void ResourceGroupLightTriangle::set_size(int n) {
    infos.reserve(n);
}

int ResourceGroupLightTriangle::n_objects() {
    return infos.size();
}

ResourceLight *ResourceGroupLightTriangle::get_info(int n) {
    return &infos[n];
}

void ResourceGroupLightTriangle::add(const ResourceLightTriangle &info) {
    infos.push_back(info);
}

void ResourceGroupLightTriangle::trans(Mat3 &T) {
    for(int i = 0; i < n_objects(); ++i) {
        get_info(i)->get_shape()->trans(T);
    }
}