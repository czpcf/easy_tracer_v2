#include "sampler_reflection.hpp"

SamplerReflection::SamplerReflection() {
}

bool SamplerReflection::sample_out(const Surface &surface, RNG *rng, const Vec3 &dir_in, Vec3 &dir_out, float &pdf) {
    pdf = -1.0f;
    Vec3 norm = surface.get_normal();
    if(dir_in.dot(norm) > 0) {
        dir_out = -dir_in + norm * (2.0f * dir_in.dot(norm));
    } else {
        dir_out = -dir_in - norm * (2.0f * dir_in.dot(norm));
    }
    return true;
}

bool SamplerReflection::sample_in(const Surface &surface, RNG *rng, const Vec3 &dir_out, Vec3 &dir_in, float &pdf) {
    pdf = -1.0f;
    Vec3 norm = surface.get_normal();
    if(dir_out.dot(norm) > 0) {
        dir_in = -dir_out + norm * (2.0f * dir_out.dot(norm));
    } else {
        dir_in = -dir_out - norm * (2.0f * dir_out.dot(norm));
    }
    return true;
}

float SamplerReflection::pdf(const Surface &surface, const Vec3 &, const Vec3 &) {
    return -1.0f;
}

bool SamplerReflection::is_specular() {
    return true;
}