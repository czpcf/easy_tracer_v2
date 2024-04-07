#include "sampler_lambertian.hpp"

SamplerLambertian::SamplerLambertian() {
}

bool SamplerLambertian::sample_out(const Surface &surface, RNG *rng, const Vec3 &dir_in, Vec3 &dir_out, float &pdf) {
    Vec3 norm = surface.get_normal();
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
    return true;
}

bool SamplerLambertian::sample_in(const Surface &surface, RNG *rng, const Vec3 &dir_out, Vec3 &dir_in, float &pdf) {
    Vec3 norm = surface.get_normal();
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
    return true;
}

float SamplerLambertian::pdf(const Surface &surface, const Vec3 &given, const Vec3 &sampled) {
    return Onb::pdf_cosine_sample_hemisphere(sampled, surface.get_normal());
}

bool SamplerLambertian::is_specular() {
    return false;
}