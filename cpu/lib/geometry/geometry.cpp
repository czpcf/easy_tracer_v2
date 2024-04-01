#include <iomanip>
#include <iostream>
#include "geometry.hpp"

Geometry::Geometry() {
}

bool Geometry::inter(Ray ray, RayHit &hit) {
    return false;
}

bool Geometry::inter_update(Ray ray, RayHit &hit) {
    return false;
}

bool Geometry::if_inter_dis(Ray ray, float dis_s) {
    return false;
}

void Geometry::trans(Mat3 &T) {
}

void Geometry::debug() {
    fprintf(stderr, "Im debugging");
}

Box Geometry::bound() {
    return Box();
}

float Geometry::area() {
    return 0.0f;
}