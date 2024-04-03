#include "resource.hpp"

Resource::Resource() {
}

Vec3 Resource::get_color(const Vec2 &, const UV &) {
    return Vec3(0.0f, 0.0f, 0.0f);
}

Vec3 Resource::get_normal(const Vec2 &, const UV &) {
    return Vec3(0.0f, 0.0f, 0.0f);
}

void Resource::get_all(const Vec2 &local, Vec3 &_color, Vec3 &_normal, Bxdf *&_bxdf, Sampler *&_sampler) {
    UV uv = local_to_uv(local);
    _color = get_color(local, uv);
    _normal = get_normal(local, uv);
    _bxdf = get_bxdf(local, uv);
    _sampler = get_sampler(local, uv);
}

ResourceGroup::ResourceGroup() {
}

Geometry *ResourceGroup::get_shape(int n) {
    return get_info(n)->get_shape();
}