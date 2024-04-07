#include <cmath>
#include "bxdf_ggx.hpp"

BxdfGGX::BxdfGGX() {
}

BxdfGGX::BxdfGGX(float rough, float ax, float ay) {
    roughness = rough;
    alpha_x = ax;
    alpha_y = ay;
}

// https://www.pbr-book.org/4ed/Reflection_Models/Roughness_Using_Microfacet_Theory#fragment-TrowbridgeReitzDistributionPublicMethods-0
Vec3 BxdfGGX::phase(const Surface &surface, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) {

    if(dir_in.dot(norm) * dir_out.dot(norm) < 0) { // not in the same hemisphere
        return Vec3(0.0f, 0.0f, 0.0f);
    }

    // approximated fresnel
    auto f = [&](float cos_theta) {
        float a = cos_theta * cos_theta;
        a = a * a;
        return 0.04f + 0.96f * (1.0f - a * cos_theta);
    };

    // distribution of normals
    auto d = [&](const Vec3 &w) {
        float cos_theta = fabs(surface.cos_theta(w));
        float cos_phi = surface.cos_phi(w);
        float cos2_phi = cos_phi * cos_phi;
        float sin2_phi = 1.0f - cos2_phi;
        float cos2_theta = cos_theta * cos_theta;
        float sin2_theta = 1.0f - cos2_theta;
        float ax2 = alpha_x * alpha_x;
        float ay2 = alpha_y * alpha_y;
        float s = cos2_theta + sin2_theta * (cos2_phi / ax2 + sin2_phi / ay2);
        return INV_PI / (alpha_x * alpha_y * s * s);
    };

    // lambda function
    auto lambda = [&](const Vec3 &w) {
        float cos_phi = surface.cos_phi(w);
        float cos_theta = fabs(surface.cos_theta(w));
        float cos2_theta = cos_theta * cos_theta;
        float tan2_theta = (1.0f - cos2_theta) / cos2_theta;
        float cos2_phi = cos_phi * cos_phi;
        float sin2_phi = 1.0f - cos2_phi;
        float alpha2 = alpha_x * alpha_x * cos2_phi + alpha_y * alpha_y * sin2_phi;
        return (std::sqrt(1.0f + alpha2 * tan2_theta) - 1.0f) * 0.5f;
    };

    // masking function
    auto mask = [&](const Vec3 &wo, const Vec3 &wi) {
        return 1.0f / (1.0f + lambda(wo) + lambda(wi));
    };

    auto x5 = [](float x){
        float a = x * x;
        return a * a * x;
    };

    Vec3 half = (dir_in + dir_out).norm();

    // modified from disney brdf
    float fd90 = 0.5f + 2.0f * surface.cos_theta(half) * roughness;
    float diffuse =
        INV_PI *
        (1.0f + (fd90 - 1.0f) * x5(1.0f - surface.cos_theta(dir_in))) *
        (1.0f + (fd90 - 1.0f) * x5(1.0f - surface.cos_theta(dir_out))) * roughness;
    
    return surface.get_color() * (d(half) * f(dir_in.dot(half)) * mask(dir_out, dir_in) /
        fabs(4.0f * surface.cos_theta(dir_in) * surface.cos_theta(dir_out)) + diffuse);
}