#include <cmath>
#include "../bxdf/bxdf.hpp"

Surface::Surface() {
}


Surface::Surface(
    const Vec3 &_normal,
    const Vec3 &_x,
    const Vec3 &_y,
    const Vec3 &_color,
    Bxdf *b,
    Sampler *s):
    normal(_normal), x(_x), y(_y), color(_color), bxdf(b), sampler(s) {
}

Bxdf *Surface::get_bxdf() {
    return bxdf;
}

Sampler *Surface::get_sampler() {
    return sampler;
}

const Vec3 Surface::get_normal() const {
    return normal;
}

const Vec3 Surface::get_x() const {
    return x;
}

const Vec3 Surface::get_y() const {
    return y;
}

const Vec3 Surface::get_color() const {
    return color;
}

const float Surface::cos_theta(const Vec3 &w) const {
    return w.dot(normal);
}

const float Surface::cos_phi(const Vec3 &w) const {
    float g = w.dot(normal);
    if(fabs(g) > 0.99995f) {
        return 0.0f;
    }
    return x.dot((w - normal * g).norm());
}

const Vec3 Surface::world_to_local(const Vec3 &w) const {
    Vec3 projected = w - normal * normal.dot(w);
    float dot_x = projected.dot(x);
    float dot_y = projected.dot(y);
    return Vec3(dot_x, dot_y, normal.dot(w)).norm();
}

const Vec3 Surface::local_to_world(const Vec3 &w) const {
    return 
        normal * w.z +
        x * w.x +
        y * w.y;
}