#include "sampler_lambertian.hpp"

SamplerLambertian::SamplerLambertian() {
}

void SamplerLambertian::sample_out(RNG *rng, const Vec3 &dir_in, const Vec3 &norm, Vec3 &dir_out, float &pdf) {
    if(dir_in.dot(norm) > 0) {
        Onb onb(norm);
        onb.cosine_sample_hemisphere(rng);
        dir_out = onb.to_world();
        pdf = onb.pdf();
    } else {
        Onb onb(-norm);
        onb.cosine_sample_hemisphere(rng);
        dir_out = onb.to_world();
        pdf = onb.pdf();
    }
}

void SamplerLambertian::sample_in(RNG *rng, const Vec3 &dir_out, const Vec3 &norm, Vec3 &dir_in, float &pdf) {
    if(dir_out.dot(norm) > 0) {
        Onb onb(norm);
        onb.cosine_sample_hemisphere(rng);
        dir_in = onb.to_world();
        pdf = onb.pdf();
    } else {
        Onb onb(-norm);
        onb.cosine_sample_hemisphere(rng);
        dir_in = onb.to_world();
        pdf = onb.pdf();
    }
}

float SamplerLambertian::pdf(const Vec3 &given, const Vec3 &sampled, const Vec3 &norm) {
    return Onb::pdf_cosine_sample_hemisphere(sampled, norm);
}

bool SamplerLambertian::is_specular() {
    return false;
}