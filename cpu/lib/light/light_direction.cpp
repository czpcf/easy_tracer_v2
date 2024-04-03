#include <cmath>
#include "light_direction.hpp"

LightDirection::LightDirection() {
}

LightDirection::LightDirection(const Direction &d): d(d) {}

bool LightDirection::can_sample_point() {
    return false;
}

void LightDirection::sample_point(RNG *rng, const Vec3 &p_on_suf, Vec3 &p_on_light, float &pdf) {
    // do nothing
    pdf = -1.0f;
}

void LightDirection::sample_direction(RNG *rng, const Vec3 &p_on_light, Vec3 &dir, float &pdf) {
    pdf = -1.0f;
    dir = d.get_d();
}

void LightDirection::sample_ray(RNG *rng, Ray &ray, float &pdf) {
    // do nothing
    pdf = -1.0f;
}

void LightDirection::sample_in_ray(RNG *rng, const Vec3 &p_on_suf, Ray &ray, float &pdf) {
    ray = Ray(p_on_suf - d.get_d() * 65535.0f, d.get_d());
    pdf = -1.0f;
}

float LightDirection::pdf_point(const Vec3&) {
    return -1.0f;
}

float LightDirection::pdf_ray(const Ray &ray_out) {
    return -1.0f;
}

Geometry *LightDirection::get_shape() {
    return &d;
}