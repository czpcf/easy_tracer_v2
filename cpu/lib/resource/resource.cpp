#include "resource.hpp"

Resource::Resource() {
}

Vec3 Resource::get_color(const Vec2 &, const UV &) {
    return Vec3(0.0f, 0.0f, 0.0f);
}

Vec3 Resource::get_normal(const Vec2 &, const UV &) {
    return Vec3(0.0f, 0.0f, 0.0f);
}

ResourceGroup::ResourceGroup() {
}

int ResourceGroup::n_objects() {
    return 0;
}