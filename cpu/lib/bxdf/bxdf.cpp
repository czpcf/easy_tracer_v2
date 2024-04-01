#include "bxdf.hpp"

Bxdf::Bxdf() {
}

Vec3 Bxdf::phase(const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) {
    return Vec3(0.0f, 0.0f, 0.0f);
}