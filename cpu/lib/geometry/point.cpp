#include <iostream>
#include "point.hpp"

Point::Point() {
}

Point::Point(const Vec3 &p): o(p) {
}

bool Point::inter(const Ray &, RayHit &) {
    return false;
}

bool Point::inter_update(const Ray &, RayHit &) {
    return false;
}

bool Point::if_inter_dis(const Ray &, float) {
    return false;
}

Vec2 Point::inter_to_local(const Vec3 &) {
    return Vec2(0.0f, 0.0f);
}

void Point::trans(Mat3 &T) {
    o = T.map(o);
}

void Point::debug() {
    std::cerr << "Point: (" << o.x << ',' << o.y << ',' << o.z << ")" << std::endl;
}

Box Point::bound() {
    return Box(o, o);
}

float Point::area() {
    return 0.0f;
}

const Vec3 Point::get_o() const {
    return o;
}