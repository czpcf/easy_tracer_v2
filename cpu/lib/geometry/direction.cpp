#include <iostream>
#include "direction.hpp"

Direction::Direction() {
}

Direction::Direction(const Vec3 &p) {
    d = p;
    d.normalize();
}

bool Direction::inter(const Ray &, RayHit &) {
    return false;
}

bool Direction::inter_update(const Ray &, RayHit &) {
    return false;
}

bool Direction::if_inter_dis(const Ray &, float) {
    return false;
}

void Direction::trans(Mat3 &T) {
    d = T.map_scale(d);
    d.normalize();
}

void Direction::debug() {
    std::cerr << "Direction: (" << d.x << ',' << d.y << ',' << d.z << ")" << std::endl;
}

Box Direction::bound() {
    return Box(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f));
}

float Direction::area() {
    return 0.0f;
}

const Vec3 Direction::get_d() const {
    return d;
}