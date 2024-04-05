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

Vec3 ResourceLightTriangle::get_color(const Vec2 &, const UV &) {
    return texture->get(UV());
}

Vec3 ResourceLightTriangle::get_normal(const Vec2 &, const UV &) {
    return light_triangle->get_norm();
}

Bxdf *ResourceLightTriangle::get_bxdf(const Vec2 &, const UV &) {
    return bxdf;
}

Sampler *ResourceLightTriangle::get_sampler(const Vec2 &, const UV &) {
    return sampler;
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