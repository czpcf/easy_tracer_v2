#include <cmath>
#include "light_point.hpp"

LightPoint::LightPoint() {
}

LightPoint::LightPoint(const Point &p): o(p) {}

void LightPoint::sample_point(RNG *rng,  Vec3 &p_on_light, float &pdf, Vec2 &local) {
    p_on_light = o.get_o();
    pdf = -1.0f;
    local = Vec2(0.0f, 0.0f);
}

void LightPoint::sample_point(RNG *rng, const Vec3 &p_on_suf, Vec3 &p_on_light, float &pdf, Vec2 &local) {
    p_on_light = o.get_o();
    pdf = -1.0f;
    local = Vec2(0.0f, 0.0f);
}

void LightPoint::sample_direction(RNG *rng, const Vec3 &p_on_light, Vec3 &dir, float &pdf) {
    // uniform sampling on unit sphere
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
    dir = Vec3(ux / lambda, uy / lambda, uz / lambda);
    pdf = 1.0f / (4.0f * PI);
}

void LightPoint::sample_ray(RNG *rng, Ray &ray, float &pdf, Vec2 &local) {
    // uniform sampling on unit sphere
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
    ray = Ray(o.get_o(), Vec3(ux / lambda, uy / lambda, uz / lambda));
    pdf = 1.0f / (4.0f * PI);
    local = Vec2(0.0f, 0.0f);
}

bool LightPoint::sample_in_ray(RNG *rng, const Vec3 &p_on_suf, Ray &ray, float &pdf, Vec2 &local) {
    ray = Ray(o.get_o(), (p_on_suf - o.get_o()).norm());
    pdf = 1.0f / (4.0f * PI);
    local = Vec2(0.0f, 0.0f);
    return true;
}

float LightPoint::pdf_point(const Vec3&) {
    return -1.0f;
}

float LightPoint::pdf_ray(const Ray &ray_out) {
    return 1.0 / (4.0 * PI);
}

Geometry *LightPoint::get_shape() {
    return &o;
}

const AreaType LightPoint::area_type() const {
    return AreaType::ZERO;
}

const DirType LightPoint::dir_type() const {
    return DirType::CONTINOUS;
}

float LightPoint::decaying(const Vec3 &p_on_suf, const Vec3 &p_on_light) {
    return 1.0f / (p_on_suf - o.get_o()).square();
}