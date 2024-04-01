#include "onb.hpp"

Onb::Onb() {
}

Onb::~Onb() {
}

Onb::Onb(Vec3 normal_g) {
    _normal = normal_g;
    if(abs(_normal.x) > abs(_normal.z)) {
        _binormal = Vec3(-_normal.y, _normal.x, 0.0f);
    } else {
        _binormal = Vec3(0.0f, -_normal.z, _normal.y);
    }
    _binormal.normalize();
    _tangent = _binormal.cross(_normal);
}

Vec3 Onb::to_world() {
    return _tangent * _local.x + _binormal * _local.y + _normal * _local.z;
}

void Onb::cosine_sample_hemisphere(RNG *rng) {
    float ux = rng->rand_float();
    float uy = rng->rand_float();
    cosine_sample_hemisphere(Vec2(ux, uy));
}

void Onb::cosine_sample_hemisphere(Vec2 uv) {
    float r = sqrt(uv.x);
    uv.y *= PI * 2.0f;
    _local = Vec3(r * cos(uv.y), r * sin(uv.y), sqrt(1.0f - uv.x));
    _pdf = std::abs(_local.z) / PI;
}

float Onb::pdf_cosine_sample_hemisphere(const Vec3 &dir_out, const Vec3 &norm) {
    return std::abs(norm.dot(dir_out));
}

float Onb::pdf() {
    return _pdf;
}