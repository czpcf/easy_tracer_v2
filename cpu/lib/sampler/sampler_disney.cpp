#include "sampler_disney.hpp"

#include<iostream>
using namespace std;

static float clamp(float z, float min, float max) {
    return fmax(min, fmin(max, z));
}

static float lerp(float x, float y, float t) {
    return x * (1.0f - t) + y * t;
}

static float ln(float x) {
    return logf(x) / LOG2_E;
}

SamplerDisney::SamplerDisney() {
}

SamplerDisney::SamplerDisney(
    float _metallic,
    float _subsurface,
    float _specular,
    float _specular_tint,
    float _roughness,
    float _anisotropic,
    float _sheen,
    float _sheen_tint,
    Vec3 _clearcoat,
    float _clearcoat_gloss
):
    sigma_m(_metallic),
    sigma_ss(_subsurface),
    sigma_s(_specular),
    sigma_st(_specular_tint),
    sigma_r(_roughness),
    sigma_a(_anisotropic),
    sigma_sh(_sheen),
    sigma_sht(_sheen_tint),
    sigma_c(_clearcoat),
    sigma_cg(_clearcoat_gloss) {
        
    sigma_r = fmax(sigma_r, 0.005f);

    pdf_d = fmin(0.8f, 1.0f - sigma_m);
    pdf_s = 1.0f / (1.0f + sigma_cg / 2.0f);
    pdf_c = 1.0f - pdf_s;
    
    alpha = sqrtf(1.0f - 0.9f * sigma_a);
    alpha_x = sigma_r * sigma_r / alpha;
    alpha_y = sigma_r * sigma_r * alpha;
    alpha_c = lerp(0.1f, 0.01f, sigma_cg); // be careful !
}

bool SamplerDisney::sample_out(const Surface &surface, RNG *rng, const Vec3 &dir_in, Vec3 &dir_out, float &pdf) {
    Vec3 norm = surface.get_normal();

    // diffuse
    if(rng->rand_float() < pdf_d) {
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
    // specular
    else if(rng->rand_float() < pdf_s) { 
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
        half = surface.local_to_world(half);
        
        if(dir_in.dot(norm) < 0) {
            half = half - norm * 2.0f;
        }
        dir_out = -dir_in + half * half.dot(dir_in) * 2.0f;
        if((dir_out.dot(norm)) * (dir_in.dot(norm)) < 0) {
            return false;
        }
        pdf = this->pdf(surface, dir_in, dir_out);
        return true;
    }
    // clearcoat
    else {
        float alpha2 = alpha_c * alpha_c;
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
    return false;
}

bool SamplerDisney::sample_in(const Surface &surface, RNG *rng, const Vec3 &dir_out, Vec3 &dir_in, float &pdf) {
    return sample_out(surface, rng, dir_out, dir_in, pdf);
}

float SamplerDisney::pdf(const Surface &surface, const Vec3 &given, const Vec3 &sampled) {

    // cosine hemisphere sample
    float pdf1 = Onb::pdf_cosine_sample_hemisphere(sampled, surface.get_normal());

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
    
    // clearcoat
    float alpha2 = alpha_c * alpha_c;
    Vec3 half = (given + sampled).norm();
    float cos_theta = surface.cos_theta(half);
    float cos2_theta = cos_theta * cos_theta;
    float sin2_theta = 1.0f - cos2_theta;
    float sin_theta = sqrtf(sin2_theta);
    
    float pdf3 = (alpha2 - 1.0f) * cos_theta;
    pdf3 /= (alpha2 * cos2_theta + sin2_theta) * ln(alpha_c) * 2.0f * PI;
    pdf3 = pdf3 / (4.0f * sampled.dot(half));

    return pdf_d * pdf1 + (1.0f - pdf_d) * (pdf_s * pdf2 + pdf_c * pdf3);
}

bool SamplerDisney::is_specular() {
    return false;
}