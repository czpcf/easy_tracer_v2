#include "resource_plane.hpp"

ResourcePlane::ResourcePlane() {
}

ResourcePlane::ResourcePlane(
    Plane *_plane,
    Sampler *s,
    Bxdf *b,
    Texture *t):
    plane(_plane),
    sampler(s),
    bxdf(b),
    texture(t) {}

UV ResourcePlane::local_to_uv(const Vec2 &local) {
    return UV(local.x, local.y);
}

Vec3 ResourcePlane::get_color(const Vec2 &local, const UV &uv) {
    return texture->get(uv);
}

Vec3 ResourcePlane::get_normal(const Vec2 &local, const UV &uv) {
    return plane->get_z();
}

Bxdf *ResourcePlane::get_bxdf(const Vec2 &local, const UV &uv) {
    return bxdf;
}

Sampler *ResourcePlane::get_sampler(const Vec2 &local, const UV &uv) {
    return sampler;
}

Geometry *ResourcePlane::get_shape() {
    return plane;
}



ResourceGroupPlane::ResourceGroupPlane() {
}

void ResourceGroupPlane::set_size(int n) {
    infos.reserve(n);
}

int ResourceGroupPlane::n_objects() {
    return infos.size();
}

Resource* ResourceGroupPlane::get_info(int n) {
    return &infos[n];
}

void ResourceGroupPlane::add(const ResourcePlane &info) {
    infos.push_back(info);
}

void ResourceGroupPlane::trans(Mat3 &T) {
    for(int i = 0; i < n_objects(); ++i) {
        get_info(i)->get_shape()->trans(T);
    }
}