#include "sampler_glass.hpp"

SamplerGlass::SamplerGlass() {
    bias = 0.2f;
    scale = 0.8f;
    power = 5.0f;
}

SamplerGlass::SamplerGlass(float eta, float bias, float scale, float power):
    eta(eta), bias(bias), scale(scale), power(power) {
}

bool SamplerGlass::sample_out(const Surface &surface, RNG *rng, const Vec3 &dir_in, Vec3 &dir_out, float &pdf) {
    pdf = -1.0f;
    Vec3 norm = surface.get_normal();
    float cos_wi = norm.dot(dir_in);
    if(cos_wi > 0.0f) { // into the material
        float g = bias + scale * powf(1.0f - std::fabs(norm.dot(dir_in)), power);
        if(rng->rand_float() < g) { // reflection
            dir_out = -dir_in + norm * (2.0f * dir_in.dot(norm));
            pdf = -g;
        } else {
            float cos_theta = sqrtf((cos_wi * cos_wi - 1.0f) / eta / eta + 1.0f);
            dir_out = -dir_in / eta  + norm * (cos_wi / eta - cos_theta);
            pdf = -(1.0f - g);
        }
    } else { // out of the material
        cos_wi = -cos_wi;
        float del = eta * eta * (cos_wi * cos_wi - 1.0f) + 1.0f;
        if(del <= 0) {
            dir_out = -dir_in + norm * (2.0f * dir_in.dot(norm));
        } else {
            float cos_theta = sqrtf(del);
            dir_out = -dir_in * eta + norm * (cos_theta - cos_wi * eta);
        }
    }
    dir_out.normalize();
    return true;
}

bool SamplerGlass::sample_in(const Surface &surface, RNG *rng, const Vec3 &dir_out, Vec3 &dir_in, float &pdf) {
    return sample_out(surface, rng, dir_out, dir_in, pdf);
}

float SamplerGlass::pdf(const Surface &surface, const Vec3 &, const Vec3 &) {
    return -1.0f;
}

bool SamplerGlass::is_specular() {
    return true;
}