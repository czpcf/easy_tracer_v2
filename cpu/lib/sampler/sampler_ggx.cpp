#include "sampler_ggx.hpp"

static float clamp(float z, float min, float max) {
    return fmax(min, fmin(max, z));
}

SamplerGGX::SamplerGGX() {
}

SamplerGGX::SamplerGGX(float _r, float _x, float _y) {
    roughness = _r;
    alpha_x = _x;
    alpha_y = _y;
    change_pdf = roughness;
}

// https://zhuanlan.zhihu.com/p/57771965,
// https://schuttejoe.github.io/post/ggximportancesamplingpart1/,
// https://zhuanlan.zhihu.com/p/682281086,
// also combines with cosine_hemisphere_sampling method with the probability of roughness
bool SamplerGGX::sample_out(const Surface &surface, RNG *rng, const Vec3 &dir_in, Vec3 &dir_out, float &pdf) {
    Vec3 norm = surface.get_normal();
    if(rng->rand_float() < change_pdf) {
        if(dir_in.dot(norm) > 0) {
            Onb onb(norm);
            onb.cosine_sample_hemisphere(rng);
            dir_out = onb.to_world();
        } else {
            Onb onb(-norm);
            onb.cosine_sample_hemisphere(rng);
            dir_out = onb.to_world();
        }
        pdf = this->pdf(surface, dir_in, dir_out);
        return true;
    }

    // ggx high light sample: sampling normal version
    // // sampling phi
    // float u1 = rng->rand_float();
    // float tan2_phi = tanf(u1 * 2.0f * PI) * alpha_y / alpha_x; // !!! be careful (y / x, not x / y)
    // tan2_phi = tan2_phi * tan2_phi;
    // float cos_phi = 1.0f / (1.0f + tan2_phi);
    // float sin_phi = 1.0f - cos_phi;
    // cos_phi = sqrtf(cos_phi);
    // sin_phi = sqrtf(sin_phi);
    // if(0.25f < u1 && u1 < 0.75f) {
    //     cos_phi = -cos_phi;
    // }
    // if(u1 > 0.5f) {
    //     sin_phi = -sin_phi;
    // }

    // float value = cos_phi * cos_phi / (alpha_x * alpha_x) + sin_phi * sin_phi / (alpha_y * alpha_y);

    // // sampling theta given phi
    // float u2 = rng->rand_float();
    // float cos2_theta = (1.0f - u2) * value / ((1.0f - value) * u2 + value);
    // float cos_theta = sqrtf(cos2_theta);
    // float sin_theta = sqrtf(1.0f - cos2_theta);

    // // get half vector
    // Vec3 half =
    //     norm * cos_theta +
    //     surface.get_x() * sin_theta * cos_phi +
    //     surface.get_y() * sin_theta * sin_phi;
    // if(dir_in.dot(norm) < 0) {
    //     half = half - norm * 2.0f;
    // }
    // dir_out = -dir_in + half * half.dot(dir_in) * 2.0f;
    // // just simply ignore the sample if the normal is invisible
    // if(dir_out.dot(norm) * dir_in.dot(norm) < 0) {
    //     return false;
    // }
    // pdf = this->pdf(surface, dir_in, dir_out);
    // return true;


    // https://arxiv.org/pdf/2306.05044v2.pdf
    // ggx high light sample: sampling visible normal version
    // visible normals do NOT imply visible out rays !
    Vec3 vh = surface.world_to_local(dir_in);
    vh.z = fabs(vh.z); // adjust this later
    vh = Vec3(vh.x * alpha_x, vh.y * alpha_y, vh.z).norm();

    float phi = 2.0f * PI * rng->rand_float();
    float u2 = rng->rand_float();
    float z = 1.0f - u2 - u2 * vh.z;
    float sin_theta = sqrtf(clamp(1.0f - z * z, 0.0f, 1.0f));
    float x = sin_theta * cosf(phi);
    float y = sin_theta * sinf(phi);
    Vec3 half = Vec3(x, y, z) + vh;

    half = Vec3(half.x * alpha_x, half.y * alpha_y, fmax(0.0f, half.z)).norm();
    // dont forget to turn it into world !
    half =
        surface.get_x() * half.x +
        surface.get_y() * half.y +
        surface.get_normal() * half.z;
    
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

bool SamplerGGX::sample_in(const Surface &surface, RNG *rng, const Vec3 &dir_out, Vec3 &dir_in, float &pdf) {
    return sample_out(surface, rng, dir_out, dir_in, pdf);
}

float SamplerGGX::pdf(const Surface &surface, const Vec3 &given, const Vec3 &sampled) {

    // cosine hemisphere sample
    float pdf1 = Onb::pdf_cosine_sample_hemisphere(sampled, surface.get_normal());

    // // ggx high light sample: sampling normal version
    // Vec3 half = (given + sampled).norm();
    // float cos_theta = surface.cos_theta(half);
    // float cos2_theta = cos_theta * cos_theta;
    // float sin2_theta = 1.0f - cos2_theta;
    // float sin_theta = sqrtf(1.0f - sin2_theta);
    // float cos_phi = surface.cos_phi(half);
    // float cos2_phi = cos_phi * cos_phi;
    // float sin2_phi = 1.0f - cos2_phi;
    // float value = cos2_phi / (alpha_x * alpha_x) + sin2_phi / (alpha_y * alpha_y);
    // float pdf2 = cos_theta;
    // float s = sin2_theta * value + cos2_theta;
    // pdf2 /= PI * alpha_x * alpha_y * s * s * 4.0f * half.dot(sampled);
    // return change_pdf * pdf1 + (1.0f - change_pdf) * fmax(pdf2, 0.001f);

    // ggx high light sample: sampling visible normal version
    Vec3 w_in = surface.world_to_local(given);
    Vec3 w_out = surface.world_to_local(sampled);
    Vec3 w_half = (w_in + w_out).norm();
    Vec3 trans_w_half = Vec3(w_half.x / alpha_x, w_half.y / alpha_y, w_half.z).norm();
    Vec3 trans_w_in = Vec3(w_in.x * alpha_x, w_in.y * alpha_y, w_in.z).norm();
    float len = sqrtf(
        w_half.x * w_half.x / (alpha_x * alpha_x) +
        w_half.y * w_half.y / (alpha_y * alpha_y) +
        w_half.z * w_half.z
    );
    float pdf2 = fabs(trans_w_half.dot(trans_w_in)) / alpha_x / alpha_y /
        (2.0f * PI * (1.0f + trans_w_in.z) * len * len * len * w_out.dot(w_half));

    return change_pdf * pdf1 + (1.0f - change_pdf) * pdf2;
}

bool SamplerGGX::is_specular() {
    return false;
}