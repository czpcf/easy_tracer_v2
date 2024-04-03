#include "resource_sphere.hpp"

ResourceSphere::ResourceSphere() {
}

ResourceSphere::ResourceSphere(
    Sphere *_sphere,
    Sampler *s,
    Bxdf *b,
    Texture *t):
    sphere(_sphere),
    sampler(s),
    bxdf(b),
    texture(t) {}

UV ResourceSphere::local_to_uv(const Vec2 &local) {
    return Vec2(local.x / PI, local.y / (2.0f * PI));
}

Vec3 ResourceSphere::get_color(const Vec2 &local, const UV &uv) {
    return texture->get(uv);
}

Vec3 ResourceSphere::get_normal(const Vec2 &local, const UV &uv) {
    float theta = local.x;
    float phi = local.y;
    return sphere->z_axis() * std::cos(theta) +
           sphere->x_axis() * std::sin(theta) * std::cos(phi) +
           sphere->y_axis() * std::sin(theta) * std::sin(phi);
}

Bxdf *ResourceSphere::get_bxdf(const Vec2 &local, const UV &uv) {
    return bxdf;
}

Sampler *ResourceSphere::get_sampler(const Vec2 &local, const UV &uv) {
    return sampler;
}

Geometry *ResourceSphere::get_shape() {
    return sphere;
}



ResourceGroupSphere::ResourceGroupSphere() {
}

void ResourceGroupSphere::set_size(int n) {
    infos.reserve(n);
}

int ResourceGroupSphere::n_objects() {
    return infos.size();
}

Resource* ResourceGroupSphere::get_info(int n) {
    return &infos[n];
}

void ResourceGroupSphere::add(const ResourceSphere &info) {
    infos.push_back(info);
}

void ResourceGroupSphere::trans(Mat3 &T) {
    for(int i = 0; i < n_objects(); ++i) {
        get_info(i)->get_shape()->trans(T);
    }
}