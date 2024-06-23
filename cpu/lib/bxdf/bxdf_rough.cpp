#include <cmath>
#include "bxdf_rough.hpp"

static float max(float x, float y) {
    return x > y ? x : y;
}

BxdfRough::BxdfRough() {
}

BxdfRough::BxdfRough(float alpha) {
    this->alpha = alpha;
}

Vec3 BxdfRough::phase(const Surface &surface, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) {
    Vec3 v = (dir_in + dir_out).norm();
    float shaded = powf(fabs(v.dot(norm)), alpha);
    return surface.get_color() * shaded;
}