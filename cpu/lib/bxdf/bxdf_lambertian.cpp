#include "bxdf_lambertian.hpp"

BxdfLambertian::BxdfLambertian() {
}

Vec3 BxdfLambertian::phase(const Surface &surface, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) {
    return surface.get_color() / PI;
}