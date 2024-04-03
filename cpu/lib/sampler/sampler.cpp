#include "sampler.hpp"

Sampler::Sampler() {
}

void Sampler::sample_out(RNG *rng, const Vec3 &dir_in, const Vec3 &norm, Vec3 &dir_out, float &pdf) {
}

void Sampler::sample_in(RNG *rng, const Vec3 &dir_out, const Vec3 &norm, Vec3 &dir_in, float &pdf) {
}

float Sampler::pdf(RNG *rng, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) {
    return 0.0f;
}

bool Sampler::is_specular() {
    return false;
}