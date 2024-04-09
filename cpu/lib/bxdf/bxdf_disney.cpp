#include <cmath>
#include "bxdf_disney.hpp"

static float lerp(float x, float y, float t) {
    return x * (1.0f - t) + y * t;
}

static float ln(float x) {
    return logf(x) / LOG2_E;
}

BxdfDisney::BxdfDisney() {
}

BxdfDisney::BxdfDisney(
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
        
    alpha = sqrtf(1.0f - 0.9f * sigma_a);
    sigma_r = fmax(sigma_r, 0.005f);
    alpha_x = sigma_r * sigma_r / alpha;
    alpha_y = sigma_r * sigma_r * alpha;
    alpha_c = lerp(0.1f, 0.01f, sigma_cg); // be careful !
}

Vec3 BxdfDisney::phase(const Surface &surface, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) {

    if(dir_in.dot(norm) * dir_out.dot(norm) < 0) { // not in the same hemisphere
        return Vec3(0.0f, 0.0f, 0.0f);
    }

    auto x5 = [](float x){
        float a = x * x;
        return a * a * x;
    };

    Vec3 color = surface.get_color();
    float lum = color.x * 0.2126f + color.y * 0.7152f + color.z * 0.0722f;
    Vec3 c_tint = color / lum; // normalize ???
    Vec3 c_s = Vec3::lerp(
        Vec3::lerp(Vec3(1.0f, 1.0f, 1.0f), c_tint, sigma_st) * 0.08f * sigma_s,
        color,
        sigma_m
    );

    Vec3 half = (dir_in + dir_out).norm();
    float cos_i = fabs(surface.cos_theta(dir_in));
    float cos_o = fabs(surface.cos_theta(dir_out));
    float cos_h = fabs(surface.cos_theta(half));
    float cos_d = fabs(dir_in.dot(half));
    float fd90 = 0.5f + 2.0f * cos_d * cos_d * sigma_r;
    float fss90 = cos_d * cos_d * sigma_r;

    // diffuse term
    auto f_d = [&](){
        return (1.0f + (fd90 - 1.0f) * x5(1.0f - cos_i)) * (1.0f + (fd90 - 1.0f) * x5(1.0f - cos_o));
    };

    // subsurface scattering
    auto f_ss = [&](){
        float fss = (1.0f + (fss90 - 1.0f) * x5(1.0f - cos_i)) * (1.0f + (fss90 - 1.0f) * x5(1.0f - cos_o));
        return 1.25f * (fss * (1.0f / (cos_i + cos_o) - 0.5f) + 0.5f);
    };

    // sheen
    auto f_sh = [&](){
        return Vec3::lerp(Vec3(1.0f, 1.0f, 1.0f), c_tint, sigma_sht) * sigma_sh * x5(1.0f - cos_d);
    };

    auto F_s = [&]() {
        return c_s + (Vec3(1.0f, 1.0f, 1.0f) - c_s) * x5(1.0f - cos_d);
    };

    auto lambda_s = [&](const Vec3 &w) {
        float cos_phi = surface.cos_phi(w);
        float cos2_phi = cos_phi * cos_phi;
        float sin2_phi = 1.0f - cos2_phi;
        float cos_theta = surface.cos_theta(w);
        float cos2_theta = cos_theta * cos_theta;
        float tan2_theta = (1.0f - cos2_theta) / cos2_theta;
        return (sqrtf(1.0f + (alpha_x * alpha_x * cos2_phi + alpha_y * alpha_y * sin2_phi) * tan2_theta) - 1.0f) * 0.5f;
    };

    auto G_s = [&](){
        return (1.0f / (1.0f + lambda_s(dir_in))) * (1.0f / (1.0f + lambda_s(dir_out)));
    };

    auto D_s = [&](){
        float cos_phi = surface.cos_phi(half);
        float cos2_phi = cos_phi * cos_phi;
        float sin2_phi = 1.0f - cos2_phi;
        float cos_theta = surface.cos_theta(half);
        float cos2_theta = cos_theta * cos_theta;
        float sin2_theta = 1.0f - cos2_theta;
        float s = sin2_theta * (cos2_phi / (alpha_x * alpha_x) + sin2_phi / (alpha_y * alpha_y)) + cos2_theta;
        return 1.0f / (PI * alpha_x * alpha_y * s * s);
    };

    auto F_c = [&](){
        return 0.04f + 0.96f * x5(1.0f - cos_d);
    };


    auto lambda_c = [&](const Vec3 &w){
        float cos_theta = surface.cos_theta(w);
        float cos2_theta = cos_theta * cos_theta;
        float tan2_theta = (1.0f - cos2_theta) / cos2_theta;
        return (sqrtf(1.0f + alpha_c * alpha_c * tan2_theta) - 1.0f) * 0.5f;
    };

    auto G_c = [&](){
        return (1.0f / (1.0f + lambda_c(dir_in))) * (1.0f / (1.0f + lambda_c(dir_out)));
    };

    auto D_c = [&](){
        float cos2 = cos_h * cos_h;
        float sin2 = 1.0f - cos2;
        float s = 2.0f * PI * ln(alpha_c) * (alpha_c * alpha_c * cos2 + sin2);
        return (alpha_c * alpha_c - 1.0f) / s;
    };

    return
        (color / PI * lerp(f_d(), f_ss(), sigma_ss) + f_sh()) * (1.0f - sigma_m) +
        F_s() * (G_s() * D_s() / (4.0f * cos_i * cos_o)) +
        sigma_c * (F_c() * G_c() * D_c() / (16.0f * cos_i * cos_o));
}
