#include <iostream>
#include <utility>
#include <cmath>
#include "../const.hpp"
#include "sphere.hpp"
#include <assert.h>

const static float EPS = 1e-6;

Sphere::Sphere() {
}

Sphere::~Sphere() {
}

Sphere::Sphere(Vec3 z_axis, Vec3 x_axis, Vec3 origin, float radius) {
    if(std::fabs(z_axis.dot(x_axis)) > EPS) {
        std::cerr << "z_axis is not orthogonal to x_axis: "
                  << "(" << z_axis.x << "," << z_axis.y << "," << z_axis.z << ") and "
                  << "(" << x_axis.x << "," << x_axis.y << "," << x_axis.z << ")." << std::endl;
        assert(0);
    }
    z = z_axis.norm();
    x = x_axis.norm();
    y = z.cross(x).norm();
    ori = origin;
    r = radius;
    _area = 4.0f * PI * r * r;
}

bool Sphere::inter(const Ray & ray, RayHit &hit) {
    Vec3 d = ray.get_direction();
    Vec3 p = ray.get_origin();
    float a = d.dot(d);
    float b = ((p - ori).dot(d)) * 2;
    float c = ((p - ori).dot(p - ori)) - r * r;
    float del = b * b - a * c * 4;
    if(del < 0) {
        return false;
    }
    float r_del = std::sqrt(del);
    float t1 = (-b - r_del) / (a * 2);
    if(t1 < 0) {
        t1 = (-b + r_del) / (a * 2);
    }
    if(t1 < 0) {
        return false;
    }
    Vec3 inter = p + d * t1;
    p = inter - ori;
    float cos_theta = p.dot(z) / r; // remember to divide by r because r may not be 1
    if(cos_theta <= -1.0 + EPS) {
        hit.set(0, t1, Vec2(PI, 0.0f));
        return true;
    } else if(cos_theta >= 1.0 - EPS) {
        hit.set(0, t1, Vec2(0.0f, 0.0f));
        return true;
    }
    float theta = std::acos(cos_theta);
    Vec3 proj = p - z * p.dot(z); // be careful
    if(std::fabs(proj.x) + std::fabs(proj.y) + std::fabs(proj.z) <= EPS) {
        hit.set(0, t1, Vec2(theta, 0.0f));
        return true;
    }
    float ax = x.angle(proj);
    float ay = y.angle(proj);
    if(ay <= PI / 2.0f) {
        hit.set(0, t1, Vec2(theta, ax));
        return true;
    }
    hit.set(0, t1, Vec2(theta, PI * 2.0f - ax));
    return true;
}

bool Sphere::inter_update(const Ray & ray, RayHit &hit) {
    Vec3 d = ray.get_direction();
    Vec3 p = ray.get_origin();
    float a = d.dot(d);
    float b = ((p - ori).dot(d)) * 2;
    float c = ((p - ori).dot(p - ori)) - r * r;
    float del = b * b - a * c * 4;
    if(del < 0) {
        return false;
    }
    float r_del = std::sqrt(del);
    float t1 = (-b - r_del) / (a * 2);
    if(t1 < 0) {
        t1 = (-b + r_del) / (a * 2);
    }
    if(t1 < 0 || t1 >= hit.get_dis()) {
        return false;
    }
    Vec3 inter = p + d * t1;
    p = inter - ori;
    float cos_theta = p.dot(z) / r; // remember to divide by r because r may not be 1
    if(cos_theta <= -1.0 + EPS) {
        hit.set(0, t1, Vec2(PI, 0.0f));
        return true;
    } else if(cos_theta >= 1.0 - EPS) {
        hit.set(0, t1, Vec2(0.0f, 0.0f));
        return true;
    }
    float theta = std::acos(cos_theta);
    Vec3 proj = p - z * p.dot(z); // be careful
    if(std::fabs(proj.x) + std::fabs(proj.y) + std::fabs(proj.z) <= EPS) {
        hit.set(0, t1, Vec2(theta, 0.0f));
        return true;
    }
    float ax = x.angle(proj);
    float ay = y.angle(proj);
    if(ay <= PI / 2.0f) {
        hit.set(0, t1, Vec2(theta, ax));
        return true;
    }
    hit.set(0, t1, Vec2(theta, PI * 2.0f - ax));
    return true;
}

bool Sphere::if_inter_dis(const Ray & ray, float dis) {
    Vec3 d = ray.get_direction();
    Vec3 p = ray.get_origin();
    float a = d.dot(d);
    float b = ((p - ori).dot(d)) * 2;
    float c = ((p - ori).dot(p - ori)) - r * r;
    float del = b * b - a * c * 4;
    if(del < 0) {
        return false;
    }
    float r_del = std::sqrt(del);
    float t1 = (-b - r_del) / (a * 2);
    if(t1 < 0) {
        t1 = (-b + r_del) / (a * 2);
    }
    if(t1 < 0) {
        return false;
    }
    if(t1 >= dis) {
        return false;
    }
    return true;
}

void Sphere::trans(Mat3 &T) {
    z = T.map_scale(z).norm();
    x = T.map_scale(x).norm();
    y = T.map_scale(y).norm();
    ori = T.map(ori);
    _area = 4.0f * PI * r * r;
}

void Sphere::debug() {
    std::cerr << "Debug Sphere: ";
    std::cerr << '(' << ori.x << ',' << ori.y << ',' << ori.z << ')' << std::endl;
    std::cerr <<  "z-axis: " << '(' << z.x << ',' << z.y << ',' << z.z << ')';
    std::cerr << " x-axis: " << '(' << x.x << ',' << x.y << ',' << x.z << ')';
    std::cerr << " y-axis: " << '(' << y.x << ',' << y.y << ',' << y.z << ')';
    std::cerr << " radius: " << r << std::endl;
}

Box Sphere::bound() {
    return Box(ori - Vec3(r, r, r), ori + Vec3(r, r, r));
}

float Sphere::area() {
    return _area;
}

const Vec3 &Sphere::z_axis() const{
    return z;
}

const Vec3 &Sphere::x_axis() const{
    return x;
}

const Vec3 &Sphere::y_axis() const{
    return y;
}