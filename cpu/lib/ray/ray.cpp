#include "ray.hpp"
Ray::Ray() {
}

Ray::Ray(Vec3 origin, Vec3 dir): p(origin), d(dir) {
}

const Vec3 &Ray::get_origin() const {
    return p;
}

const Vec3 &Ray::get_direction() const {
    return d;
}
