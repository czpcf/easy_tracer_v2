#include "bxdf_specular.hpp"

BxdfSpecular::BxdfSpecular() {
}

Vec3 BxdfSpecular::phase(const Surface &surface, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) {
    return surface.get_color();
}