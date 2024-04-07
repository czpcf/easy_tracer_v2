#include <cmath>
#include "bxdf_phong.hpp"

static float max(float x, float y) {
    return x > y ? x : y;
}

BxdfPhong::BxdfPhong() {
}

BxdfPhong::BxdfPhong(const Vec3 &_diffuse, const Vec3 &_specular, float _shininess) {
    diffuse = _diffuse;
    specular = _specular;
    shininess = _shininess;
}

Vec3 BxdfPhong::phase(const Surface &surface, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) {
    Vec3 shaded = Vec3(0.0f, 0.0f, 0.0f);
    shaded += diffuse * max(0.0f, norm.dot(dir_in));
    Vec3 reflect = norm * norm.dot(dir_in) * 2.0f - dir_in;
    shaded += specular * std::pow(max(0.0f, reflect.dot(dir_out)), shininess);
    return shaded * surface.get_color();
}