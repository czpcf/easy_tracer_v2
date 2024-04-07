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

Surface ResourcePlane::get_surface(const Vec2 &local) {
    UV uv = local_to_uv(local);
    return Surface(
        plane->get_z(),
        plane->get_x(),
        plane->get_y(),
        texture->get(uv),
        bxdf,
        sampler
    );
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