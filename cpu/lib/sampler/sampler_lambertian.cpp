#include "sampler_lambertian.hpp"

SamplerLambertian::SamplerLambertian() {
}

void SamplerLambertian::sample_out(RNG *rng, const Vec3 &dir_in, const Vec3 &norm, Vec3 &dir_out, float &pdf) {
    Onb onb(norm);
    onb.cosine_sample_hemisphere(rng);
    dir_out = onb.to_world();
    pdf = onb.pdf();
}

void SamplerLambertian::sample_in(RNG *rng, const Vec3 &dir_out, const Vec3 &norm, Vec3 &dir_in, float &pdf) {
    Onb onb(norm);
    onb.cosine_sample_hemisphere(rng);
    dir_in = onb.to_world();
    pdf = onb.pdf();
}

float SamplerLambertian::pdf(RNG *rng, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) {
    return Onb::pdf_cosine_sample_hemisphere(dir_out, norm);
}

bool SamplerLambertian::is_specular() {
    return false;
}