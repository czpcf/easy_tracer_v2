#include <iostream>
#include <utility>
#include <cmath>
#include <assert.h>
#include "../const.hpp"
#include "triangle.hpp"

const static float EPS = 1e-6;

Triangle::Triangle() {
}

Triangle::Triangle(Vec3 point1, Vec3 point2, Vec3 point3): p1(point1), p2(point2), p3(point3) {
    /// counter-clockwise: p1 -> p2 -> p3
    norm = ((p2 - p1).cross(p3 - p1)).norm();
    float x1 = p1.x, y1 = p1.y;
    float x2 = p2.x, y2 = p2.y;
    float x3 = p3.x, y3 = p3.y;
    float dx12 = x2 - x1, dy12 = y2 - y1;
    float dx13 = x3 - x1, dy13 = y3 - y1;
    bool small_xy = std::abs(dx12 * dy13 - dx13 * dy12) < EPS;
    float z1 = p1.z;
    float z2 = p2.z;
    float z3 = p3.z;
    float dz12 = z2 - z1;
    float dz13 = z3 - z1;
    bool small_xz = std::abs(dx12 * dz13 - dx13 * dz12) < EPS;
    // if in x-z plane or y-z plane
    if(small_xy) {
        // if in y-z plane
        if(small_xz) {
            type = 2;
        } else {
            type = 1;
        }
    } else {
        type = 0;
    }
    _area = (p2 - p1).cross(p3 - p1).len() / 2.0f;
}

bool Triangle::inter(Ray ray, RayHit &hit) {
    Vec3 d = ray.get_direction();
    Vec3 p = ray.get_origin();
    float t = d.dot(norm);
    if(std::abs(t) < EPS) {
        return false;
    }
    t = ((p1 - p).dot(norm)) / t;
    if(std::isnan(t) || t < 0) {
        return false;
    }
    Vec3 inter = p + d * t;
    float x = inter.x, y = inter.y, z = inter.z;
    float s1, s2, s3, t1, t3, sum;
    switch(type) {
        case 2:
            s1 = (p1.y - y) * (p2.z - z) - (p2.y - y) * (p1.z - z);
            s2 = (p2.y - y) * (p3.z - z) - (p3.y - y) * (p2.z - z);
            s3 = (p3.y - y) * (p1.z - z) - (p1.y - y) * (p3.z - z);
            if((s1 < 0 && s2 < 0 && s3 < 0) || (s1 > 0 && s2 > 0 && s3 > 0)) { // don't equal 0, in case very far away
                sum = s1 + s2 + s3;
                t1 = s1 / sum;
                t3 = s3 / sum; // think: why?
                hit.set(0, t, Vec2(t1, t3));
                return true;
            }
            return false;
        break;
        case 1:
            s1 = (p1.x - x) * (p2.z - z) - (p2.x - x) * (p1.z - z);
            s2 = (p2.x - x) * (p3.z - z) - (p3.x - x) * (p2.z - z);
            s3 = (p3.x - x) * (p1.z - z) - (p1.x - x) * (p3.z - z);
            if((s1 < 0 && s2 < 0 && s3 < 0) || (s1 > 0 && s2 > 0 && s3 > 0)) {
                sum = s1 + s2 + s3;
                t1 = s1 / sum;
                t3 = s3 / sum;
                hit.set(0, t, Vec2(t1, t3));
                return true;
            }
            return false;
        break;
        case 0:
            s1 = (p1.x - x) * (p2.y - y) - (p2.x - x) * (p1.y - y);
            s2 = (p2.x - x) * (p3.y - y) - (p3.x - x) * (p2.y - y);
            s3 = (p3.x - x) * (p1.y - y) - (p1.x - x) * (p3.y - y);
            if((s1 < 0 && s2 < 0 && s3 < 0) || (s1 > 0 && s2 > 0 && s3 > 0)) {
                sum = s1 + s2 + s3;
                t1 = s1 / sum;
                t3 = s3 / sum;
                hit.set(0, t, Vec2(t1, t3));
                return true;
            }
            return false;
        break;
        default:
            assert(0);
    }
    return false;
}

bool Triangle::if_inter_dis(Ray ray, float dis) {
    Vec3 d = ray.get_direction();
    Vec3 p = ray.get_origin();
    float t = d.dot(norm);
    if(std::abs(t) < EPS) {
        return false;
    }
    t = ((p1 - p).dot(norm)) / t;
    if(std::isnan(t) || t < 0) {
        return false;
    }
    if(t * t >= dis) {
        return false;
    }
    Vec3 inter = p + d * t;
    float x = inter.x, y = inter.y, z = inter.z;
    float s1, s2, s3;
    switch(type) {
        case 2:
            s1 = (p1.y - y) * (p2.z - z) - (p2.y - y) * (p1.z - z);
            s2 = (p2.y - y) * (p3.z - z) - (p3.y - y) * (p2.z - z);
            s3 = (p3.y - y) * (p1.z - z) - (p1.y - y) * (p3.z - z);
            if((s1 < 0 && s2 < 0 && s3 < 0) || (s1 > 0 && s2 > 0 && s3 > 0)) {
                return true;
            }
            return false;
        break;
        case 1:
            s1 = (p1.x - x) * (p2.z - z) - (p2.x - x) * (p1.z - z);
            s2 = (p2.x - x) * (p3.z - z) - (p3.x - x) * (p2.z - z);
            s3 = (p3.x - x) * (p1.z - z) - (p1.x - x) * (p3.z - z);
            if((s1 < 0 && s2 < 0 && s3 < 0) || (s1 > 0 && s2 > 0 && s3 > 0)) {
                return true;
            }
            return false;
        break;
        case 0:
            s1 = (p1.x - x) * (p2.y - y) - (p2.x - x) * (p1.y - y);
            s2 = (p2.x - x) * (p3.y - y) - (p3.x - x) * (p2.y - y);
            s3 = (p3.x - x) * (p1.y - y) - (p1.x - x) * (p3.y - y);
            if((s1 < 0 && s2 < 0 && s3 < 0) || (s1 > 0 && s2 > 0 && s3 > 0)) {
                return true;
            }
            return false;
        break;
        default:
            assert(0);
    }
    return false;
}

void Triangle::trans(Mat3&T) {
    p1 = T.map(p1);
    p2 = T.map(p2);
    p3 = T.map(p3);
    norm = ((p2 - p1).cross(p3 - p1)).norm();
    float x1 = p1.x, y1 = p1.y;
    float x2 = p2.x, y2 = p2.y;
    float x3 = p3.x, y3 = p3.y;
    float dx12 = x2 - x1, dy12 = y2 - y1;
    float dx13 = x3 - x1, dy13 = y3 - y1;
    bool small_xy = std::abs(dx12 * dy13 - dx13 * dy12) < EPS;
    float z1 = p1.z;
    float z2 = p2.z;
    float z3 = p3.z;
    float dz12 = z2 - z1;
    float dz13 = z3 - z1;
    bool small_xz = std::abs(dx12 * dz13 - dx13 * dz12) < EPS;
    // if in x-z plane or y-z plane
    if(small_xy) {
        // if in y-z plane
        if(small_xz) {
            type = 2;
        } else {
            type = 1;
        }
    } else {
        type = 0;
    }
    _area = (p2 - p1).cross(p3 - p1).len() / 2.0f;
}

void Triangle::debug() {
    std::cerr << "Debug Triangle: ";
    std::cerr << '(' << p1.x << ',' << p1.y << ',' << p1.z << ')';
    std::cerr << '(' << p2.x << ',' << p2.y << ',' << p2.z << ')';
    std::cerr << '(' << p3.x << ',' << p3.y << ',' << p3.z << ')' << std::endl;
    std::cerr << "          Norm: " << norm.x << ' ' << norm.y << ' ' << norm.z << std::endl;
}

Box Triangle::bound() {
    Box b(p1, p2);
    b.add(p3);
    return b;
}

float Triangle::area() {
    return _area;
}
