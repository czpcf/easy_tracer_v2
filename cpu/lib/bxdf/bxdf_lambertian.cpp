#include "bxdf_lambertian.hpp"

BxdfLambertian::BxdfLambertian() {
}

BxdfLambertian::BxdfLambertian(float n) {
    alpha = n;
}

float BxdfLambertian::phase(const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) {
    return alpha;
}
