#include "sampler_gtr1.hpp"
#include<iostream>
using namespace std;

static float ln(float x) {
    return logf(x) / LOG2_E;
}

SamplerGTR1::SamplerGTR1() {
}

SamplerGTR1::SamplerGTR1(float _alpha) {
    alpha = _alpha;
    alpha2 = alpha * alpha;
}

// https://zhuanlan.zhihu.com/p/57771965,
// https://schuttejoe.github.io/post/ggximportancesamplingpart1/,
// https://zhuanlan.zhihu.com/p/682281086
bool SamplerGTR1::sample_out(const Surface &surface, RNG *rng, const Vec3 &dir_in, Vec3 &dir_out, float &pdf) {

    Vec3 norm = surface.get_normal();

    float phi = 2.0f * rng->rand_float() * PI;
    float cos2_theta = (powf(alpha2, 1.0f - rng->rand_float()) - 1.0f) / (alpha2 - 1.0f);
    float cos_theta = sqrtf(cos2_theta);
    float sin_theta = sqrtf(1.0f - cos2_theta);

    Vec3 half(sin_theta * cosf(phi), sin_theta * sinf(phi), cos_theta);
    half = surface.local_to_world(half);
    
    if(dir_in.dot(norm) < 0) {
        half = half - norm * 2.0f;
    }
    dir_out = -dir_in + half * half.dot(dir_in) * 2.0f;
    if((dir_out.dot(surface.get_normal())) * (dir_in.dot(surface.get_normal())) < 0) {
        return false;
    }
    pdf = this->pdf(surface, dir_in, dir_out);
    return true;
}

bool SamplerGTR1::sample_in(const Surface &surface, RNG *rng, const Vec3 &dir_out, Vec3 &dir_in, float &pdf) {
    return sample_out(surface, rng, dir_out, dir_in, pdf);
}

float SamplerGTR1::pdf(const Surface &surface, const Vec3 &given, const Vec3 &sampled) {

    Vec3 half = (given + sampled).norm();
    float cos_theta = surface.cos_theta(half);
    float cos2_theta = cos_theta * cos_theta;
    float sin2_theta = 1.0f - cos2_theta;
    float sin_theta = sqrtf(sin2_theta);
    
    float s = (alpha2 - 1.0f) * cos_theta;
    s /= (alpha2 * cos2_theta + sin2_theta) * ln(alpha) * 2.0f * PI;

    return fmax(s / (4.0f * sampled.dot(half)), 0.003f);
}

bool SamplerGTR1::is_specular() {
    return false;
}