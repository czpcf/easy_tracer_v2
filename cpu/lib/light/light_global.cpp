#include <cmath>
#include "light_global.hpp"

LightGlobal::LightGlobal() {
    p = Global();
}

void LightGlobal::sample_point(RNG *rng, Vec3 &p_on_light, float &pdf, Vec2 &local) {
    // do nothing
    pdf = -1.0f;
    local = Vec2(0.0f, 0.0f);
}

void LightGlobal::sample_point(RNG *rng, const Vec3 &p_on_suf, Vec3 &p_on_light, float &pdf, Vec2 &local) {
    // do nothing
    pdf = -1.0f;
    local = Vec2(0.0f, 0.0f);
}

void LightGlobal::sample_direction(RNG *rng, const Vec3 &p_on_light, Vec3 &dir, float &pdf) {
    // do nothing
    pdf = -1.0f;
}

void LightGlobal::sample_ray(RNG *rng, Ray &ray, float &pdf, Vec2 &local) {
    // do nothing
    pdf = -1.0f;
    local = Vec2(0.0f, 0.0f);
}

bool LightGlobal::sample_in_ray(RNG *rng, const Vec3 &p_on_suf, Ray &ray, float &pdf, Vec2 &local) {
    float ux = rng->rand_float() * 2.0f - 1.0f;
    float uy = rng->rand_float() * 2.0f - 1.0f;
    float uz = rng->rand_float() * 2.0f - 1.0f;
    float lambda = ux * ux + uy * uy + uz * uz;
    while(lambda > 1.0f) { // about 2 times in expectance
        ux = rng->rand_float() * 2.0f - 1.0f;
        uy = rng->rand_float() * 2.0f - 1.0f;
        uz = rng->rand_float() * 2.0f - 1.0f;
        lambda = ux * ux + uy * uy + uz * uz;
    }
    lambda = std::sqrt(lambda);
    Vec3 dir = Vec3(ux / lambda, uy / lambda, uz / lambda);
    ray = Ray(p_on_suf - dir * 1024.0f, dir);
    pdf = 1.0f / (4.0f * PI);
    local = Vec2(0.0f, 0.0f);
    return true;
}

float LightGlobal::pdf_point(const Vec3&) {
    return -1.0f;
}

float LightGlobal::pdf_ray(const Ray &ray_out) {
    return -1.0f;
}

Geometry *LightGlobal::get_shape() {
    return &p;
}

const AreaType LightGlobal::area_type() const {
    return AreaType::INFINITE;
}

const DirType LightGlobal::dir_type() const {
    return DirType::CONTINOUS;
}

float LightGlobal::decaying(const Vec3 &p_on_suf, const Vec3 &p_on_light) {
    return 1.0f;
}