#include "sampler.hpp"

void Sampler::sample_out(RNG *rng, const Vec3 &dir_in, const Vec3 &norm, Vec3 &dir_out, float &pdf) {
}

void Sampler::sample_in(RNG *rng, const Vec3 &dir_out, const Vec3 &norm, Vec3 &dir_in, float &pdf) {
}

void Sampler::pdf(RNG *rng, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm, float &pdf) {
}

bool Sampler::is_specular() {
    return false;
}