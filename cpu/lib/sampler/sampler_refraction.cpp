#include "sampler_refraction.hpp"

SamplerRefraction::SamplerRefraction() {
    n = 1.5f;
}

SamplerRefraction::SamplerRefraction(float eta) {
    n = eta;
}

void SamplerRefraction::sample_out(RNG *rng, const Vec3 &dir_in, const Vec3 &norm, Vec3 &dir_out, float &pdf) {
    pdf = -1.0f;
    float cos_wi = norm.dot(dir_in);
    if(cos_wi > 0.0f) { // into the material
        float cos_theta = std::sqrt((cos_wi * cos_wi - 1.0f) / n / n + 1.0f);
        dir_out = -dir_in / n  + norm * (cos_wi / n - cos_theta);
    } else { // out of the material
        cos_wi = -cos_wi;
        float del = n * n * (cos_wi * cos_wi - 1.0f) + 1.0f;
        if(del <= 0) {
            dir_out = -dir_in + norm * (2.0f * dir_in.dot(norm));
        } else {
            float cos_theta = std::sqrt(del);
            dir_out = -dir_in * n + norm * (cos_theta - cos_wi * n);
        }
    }
    dir_out.normalize();
}

void SamplerRefraction::sample_in(RNG *rng, const Vec3 &dir_out, const Vec3 &norm, Vec3 &dir_in, float &pdf) {
    sample_out(rng, dir_out, norm, dir_in, pdf);
}

float SamplerRefraction::pdf(const Vec3 &, const Vec3 &, const Vec3 &norm) {
    return 0.0f;
}

bool SamplerRefraction::is_specular() {
    return true;
}