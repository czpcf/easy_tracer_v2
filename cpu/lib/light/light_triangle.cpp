#include <cmath>
#include "light_triangle.hpp"
#include "../sampler/onb.hpp"

LightTriangle::LightTriangle() {
}

LightTriangle::LightTriangle(const Triangle &t): triangle(t) {}

void LightTriangle::sample_point(RNG *rng, const Vec3 &p_on_suf, Vec3 &p_on_light, float &pdf, Vec2 &local) {
    float ux_light = rng->rand_float();
    float uy_light = rng->rand_float();
    Vec3 p1 = triangle.get_p1();
    Vec3 p2 = triangle.get_p2();
    Vec3 p3 = triangle.get_p3();
    p_on_light = p1; // mean sampling on a triangle, be careful!
    if(ux_light + uy_light > 1.0f) { // flip it into the triangle
        p_on_light += (p2 - p1) * (1.0f - ux_light) + (p3 - p1) * (1.0f - uy_light);
    } else {
        p_on_light += (p2 - p1) * ux_light + (p3 - p1) * uy_light;
    }
    pdf = 1.0f / triangle.area();
    local = get_shape()->inter_to_local(p_on_light);
}

void LightTriangle::sample_direction(RNG *rng, const Vec3 &p_on_light, Vec3 &dir, float &pdf) {
    Onb onb(triangle.get_norm());
    onb.cosine_sample_hemisphere(Vec2(rng->rand_float(), rng->rand_float()));
    pdf = onb.pdf();
    dir = onb.to_world();
}

void LightTriangle::sample_ray(RNG *rng, Ray &ray, float &pdf, Vec2 &local) {
    // position
    float ux_light = rng->rand_float();
    float uy_light = rng->rand_float();
    Vec3 p1 = triangle.get_p1();
    Vec3 p2 = triangle.get_p2();
    Vec3 p3 = triangle.get_p3();
    Vec3 p_on_light = p1; // mean sampling on a triangle, be careful!
    if(ux_light + uy_light > 1.0f) { // flip it into the triangle
        p_on_light += (p2 - p1) * (1.0f - ux_light) + (p3 - p1) * (1.0f - uy_light);
    } else {
        p_on_light += (p2 - p1) * ux_light + (p3 - p1) * uy_light;
    }
    pdf = 1.0f / triangle.area();

    // direction
    Onb onb(triangle.get_norm());
    onb.cosine_sample_hemisphere(Vec2(rng->rand_float(), rng->rand_float()));
    pdf *= onb.pdf();
    Vec3 dir = onb.to_world();

    ray = Ray(p_on_light, dir);
    local = get_shape()->inter_to_local(p_on_light);
}

bool LightTriangle::sample_in_ray(RNG *rng, const Vec3 &p_on_suf, Ray &ray, float &pdf, Vec2 &local) {
    Vec3 p_on_light;
    sample_point(rng, p_on_suf, p_on_light, pdf, local);
    Vec3 dir = (p_on_suf - p_on_light).norm(); // may be faster ?
    if(dir.dot(triangle.get_norm()) < 0) {
        return false;
    }
    ray = Ray(p_on_light, dir);
    return true;
}

float LightTriangle::pdf_point(const Vec3&) {
    return 1.0f / triangle.area();
}

float LightTriangle::pdf_ray(const Ray &ray_out) {
    if(triangle.get_norm().dot(ray_out.get_direction()) < 0) {
        return 0.0f;
    }
    return 1.0 / (4.0 * PI) * Onb::pdf_cosine_sample_hemisphere(ray_out.get_direction(), triangle.get_norm());
}

Geometry *LightTriangle::get_shape() {
    return &triangle;
}

bool LightTriangle::is_specular() {
    return false;
}

float LightTriangle::decaying(const Vec3 &p_on_suf, const Vec3 &p_on_light) {
    return 1.0f / (p_on_suf - p_on_light).square();
}

const Vec3 LightTriangle::get_norm() const {
    return triangle.get_norm();
}