#include <iostream>
#include "global.hpp"

const static float DIS = 4096.0f;

Global::Global() {
}

bool Global::inter(const Ray &, RayHit &hit) {
    hit.set(0, DIS, Vec2(0.0f, 0.0f));
    return true;
}

bool Global::inter_update(const Ray &, RayHit &hit) {
    if(hit.get_dis() > DIS) {
        hit.set(0, DIS, Vec2(0.0f, 0.0f));
        return true;
    }
    return false;
}

bool Global::if_inter_dis(const Ray &, float t) {
    if(DIS < t) {
        return true;
    }
    return false;
}

Vec2 Global::inter_to_local(const Vec3 &) {
    return Vec2(0.0f, 0.0f);
}

void Global::trans(Mat3 &T) {
}

void Global::debug() {
    std::cerr << "Global" << std::endl;
}

Box Global::bound() {
    return Box(Vec3(-DIS, -DIS, -DIS), Vec3(DIS, DIS, DIS));
}

float Global::area() {
    return 1e30;
}
