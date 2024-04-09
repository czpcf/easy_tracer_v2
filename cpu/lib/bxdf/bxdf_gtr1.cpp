#include <cmath>
#include "bxdf_gtr1.hpp"

static float ln(float x) {
    return logf(x) / LOG2_E;
}

BxdfGTR1::BxdfGTR1() {
}

BxdfGTR1::BxdfGTR1(float _alpha) {
    alpha = _alpha;
    alpha2 = alpha * alpha;
}

Vec3 BxdfGTR1::phase(const Surface &surface, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) {

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
        float cos2_theta = cos_theta * cos_theta;
        float sin2_theta = 1.0f - cos2_theta;
        float s = 2.0f * PI * ln(alpha) * (alpha2 * cos2_theta + sin2_theta);
        return (alpha2 - 1.0f) / s;
    };

    // lambda function
    auto lambda = [&](const Vec3 &w) {
        float cos_theta = fabs(surface.cos_theta(w));
        float cos2_theta = cos_theta * cos_theta;
        float tan2_theta = (1.0f - cos2_theta) / cos2_theta;
        return (std::sqrt(1.0f + alpha2 * tan2_theta) - 1.0f) * 0.5f;
    };

    // masking function
    auto mask = [&](const Vec3 &wo, const Vec3 &wi) {
        return 1.0f / (1.0f + lambda(wo) + lambda(wi));
    };

    Vec3 half = (dir_in + dir_out).norm();
    
    return surface.get_color() * (d(half) * f(dir_in.dot(half)) * mask(dir_out, dir_in) /
        fabs(4.0f * surface.cos_theta(dir_in) * surface.cos_theta(dir_out)));
}

Vec3 BxdfGTR1::phase(
        float _alpha,
        const Surface &surface, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm
) {
    if(dir_in.dot(norm) * dir_out.dot(norm) < 0) { // not in the same hemisphere
        return Vec3(0.0f, 0.0f, 0.0f);
    }

    float _alpha2 = _alpha * _alpha;

    // approximated fresnel
    auto f = [&](float cos_theta) {
        float a = cos_theta * cos_theta;
        a = a * a;
        return 0.04f + 0.96f * (1.0f - a * cos_theta);
    };

    // distribution of normals
    auto d = [&](const Vec3 &w) {
        float cos_theta = fabs(surface.cos_theta(w));
        float cos2_theta = cos_theta * cos_theta;
        float sin2_theta = 1.0f - cos2_theta;
        float s = 2.0f * PI * ln(_alpha) * (_alpha2 * cos2_theta + sin2_theta);
        return (_alpha2 - 1.0f) / s;
    };

    // lambda function
    auto lambda = [&](const Vec3 &w) {
        float cos_theta = fabs(surface.cos_theta(w));
        float cos2_theta = cos_theta * cos_theta;
        float tan2_theta = (1.0f - cos2_theta) / cos2_theta;
        return (std::sqrt(1.0f + _alpha2 * tan2_theta) - 1.0f) * 0.5f;
    };

    // masking function
    auto mask = [&](const Vec3 &wo, const Vec3 &wi) {
        return 1.0f / (1.0f + lambda(wo) + lambda(wi));
    };

    Vec3 half = (dir_in + dir_out).norm();
    
    return surface.get_color() * (d(half) * f(dir_in.dot(half)) * mask(dir_out, dir_in) /
        fabs(4.0f * surface.cos_theta(dir_in) * surface.cos_theta(dir_out)));
}