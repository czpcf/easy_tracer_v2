#include <iostream>
#include <cmath>
#include "plane.hpp"

const static float EPS = 1e-6;

Plane::Plane() {
}

Plane::Plane(const Vec3 &ori, const Vec3 &_z, const Vec3 &_x, const Vec3 &_y) {
    origin = ori;
    z = _z;
    z.normalize();

    x = _x;
    x = (x - (x * z.dot(x))).norm();

    y = z.cross(x).norm();
}

bool Plane::inter(const Ray &ray, RayHit &hit) {
    Vec3 p = ray.get_origin();
    Vec3 d = ray.get_direction();
    float tdis = z.dot(origin - p);
    float g = z.dot(d);
    if(std::fabs(g) < EPS) {
        return false;
    }
    tdis /= g;
    if(tdis < EPS) {
        return false;
    }
    Vec3 inter = p + d * tdis;
    hit.set(0, tdis, Vec2((inter - origin).dot(x), (inter - origin).dot(y)));
    return true;
}

bool Plane::inter_update(const Ray &ray, RayHit &hit) {
    Vec3 p = ray.get_origin();
    Vec3 d = ray.get_direction();
    float tdis = z.dot(origin - p);
    float g = z.dot(d);
    if(std::fabs(g) < EPS) {
        return false;
    }
    tdis /= g;
    if(tdis < EPS || tdis >= hit.get_dis()) {
        return false;
    }
    Vec3 inter = p + d * tdis;
    hit.set(0, tdis, Vec2((inter - origin).dot(x), (inter - origin).dot(y)));
    return true;
}

bool Plane::if_inter_dis(const Ray &ray, float dis) {
    Vec3 p = ray.get_origin();
    Vec3 d = ray.get_direction();
    float tdis = z.dot(origin - p);
    float g = z.dot(d);
    if(std::abs(g) < EPS) {
        return false;
    }
    tdis /= g;
    return tdis < dis;
}

Vec2 Plane::inter_to_local(const Vec3 &inter) {
    return Vec2((inter - origin).dot(x), (inter - origin).dot(y));
}

void Plane::trans(Mat3 &T) {
    origin = T.map(origin);
    z = T.map_scale(z).norm();
    x = T.map_scale(x).norm();
    y = T.map_scale(y).norm();
}

void Plane::debug() {
    std::cerr << "Plane: origin: (" << origin.x << ',' << origin.y << ',' << origin.z << ")" << std::endl;
    std::cerr << "z: (" << z.x << ',' << z.y << ',' << z.z << ')' << std::endl;
    std::cerr << "x: (" << x.x << ',' << x.y << ',' << x.z << ')' << std::endl;
    std::cerr << "y: (" << y.x << ',' << y.y << ',' << y.z << ')' << std::endl;
}

Box Plane::bound() {
    // infinitely large
    return Box(Vec3(-1e38, -1e38, -1e38), Vec3(1e38, 1e38, 1e38));
}

float Plane::area() {
    // infinitely large
    return 1e38;
}

const Vec3 &Plane::get_z() const {
    return z;
}

const Vec3 &Plane::get_x() const {
    return x;
}

const Vec3 &Plane::get_y() const {
    return y;
}