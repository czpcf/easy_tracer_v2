#include "sampler.hpp"

Sampler::Sampler() {
}

bool Sampler::sample_out(const Surface &surface, RNG *rng, const Vec3 &dir_in, Vec3 &dir_out, float &pdf) {
    return false;
}

bool Sampler::sample_in(const Surface &surface, RNG *rng, const Vec3 &dir_out, Vec3 &dir_in, float &pdf) {
    return false;
}

float Sampler::pdf(const Surface &surface, const Vec3 &dir_in, const Vec3 &dir_out) {
    return 0.0f;
}

bool Sampler::is_specular() {
    return true;
}