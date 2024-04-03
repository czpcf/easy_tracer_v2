#include "sampler_reflection.hpp"

SamplerReflection::SamplerReflection() {
}

void SamplerReflection::sample_out(RNG *rng, const Vec3 &dir_in, const Vec3 &norm, Vec3 &dir_out, float &pdf) {
    pdf = -1.0f;
    dir_out = -dir_in + norm * (2.0f * dir_out.dot(norm));
}

void SamplerReflection::sample_in(RNG *rng, const Vec3 &dir_out, const Vec3 &norm, Vec3 &dir_in, float &pdf) {
    pdf = -1.0f;
    dir_in = -dir_out + norm * (2.0f * dir_in.dot(norm));
}

float SamplerReflection::pdf(RNG *rng, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) {
    return 0.0f;
}

bool SamplerReflection::is_specular() {
    return true;
}