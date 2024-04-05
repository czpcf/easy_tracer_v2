#include "bxdf_lambertian.hpp"

BxdfLambertian::BxdfLambertian() {
}

BxdfLambertian::BxdfLambertian(const Vec3 &n) {
    alpha = n;
}

Vec3 BxdfLambertian::phase(const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) {
    return alpha / PI;
}