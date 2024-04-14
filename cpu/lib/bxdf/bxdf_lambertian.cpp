#include "bxdf_lambertian.hpp"

BxdfLambertian::BxdfLambertian() {
}

Vec3 BxdfLambertian::phase(const Surface &surface, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) {
    if(norm.dot(dir_in) * norm.dot(dir_out) < 0) {
        return Vec3(0.0f, 0.0f, 0.0f);
    }
    return surface.get_color() / PI;
}