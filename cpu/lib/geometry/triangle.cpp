#include <iostream>
#include <utility>
#include <cmath>
#include <assert.h>
#include "../const.hpp"
#include "triangle.hpp"

const static float EPS = 1e-6;

Triangle::Triangle() {
}

Triangle::Triangle(Vec3 point1, Vec3 point2, Vec3 point3): _p1(point1), _p2(point2), _p3(point3) {
    /// counter-clockwise: p1 -> p2 -> p3
    norm = ((_p2 - _p1).cross(_p3 - _p1)).norm();
    float x1 = _p1.x, y1 = _p1.y;
    float x2 = _p2.x, y2 = _p2.y;
    float x3 = _p3.x, y3 = _p3.y;
    float dx12 = x2 - x1, dy12 = y2 - y1;
    float dx13 = x3 - x1, dy13 = y3 - y1;
    bool small_xy = std::fabs(dx12 * dy13 - dx13 * dy12) < EPS;
    float z1 = _p1.z;
    float z2 = _p2.z;
    float z3 = _p3.z;
    float dz12 = z2 - z1;
    float dz13 = z3 - z1;
    bool small_xz = std::fabs(dx12 * dz13 - dx13 * dz12) < EPS;
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
    _area = (_p2 - _p1).cross(_p3 - _p1).len() / 2.0f;
}

bool Triangle::inter(const Ray & ray, RayHit &hit) {
    Vec3 d = ray.get_direction();
    Vec3 p = ray.get_origin();
    float t = d.dot(norm);
    if(std::fabs(t) < EPS) {
        return false;
    }
    t = ((_p1 - p).dot(norm)) / t;
    if(std::isnan(t) || t < 0) {
        return false;
    }
    Vec3 inter = p + d * t;
    float x = inter.x, y = inter.y, z = inter.z;
    float s1, s2, s3, t1, t3, sum;
    switch(type) {
        case 2:
            s1 = (_p1.y - y) * (_p2.z - z) - (_p2.y - y) * (_p1.z - z);
            s2 = (_p2.y - y) * (_p3.z - z) - (_p3.y - y) * (_p2.z - z);
            s3 = (_p3.y - y) * (_p1.z - z) - (_p1.y - y) * (_p3.z - z);
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
            s1 = (_p1.x - x) * (_p2.z - z) - (_p2.x - x) * (_p1.z - z);
            s2 = (_p2.x - x) * (_p3.z - z) - (_p3.x - x) * (_p2.z - z);
            s3 = (_p3.x - x) * (_p1.z - z) - (_p1.x - x) * (_p3.z - z);
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
            s1 = (_p1.x - x) * (_p2.y - y) - (_p2.x - x) * (_p1.y - y);
            s2 = (_p2.x - x) * (_p3.y - y) - (_p3.x - x) * (_p2.y - y);
            s3 = (_p3.x - x) * (_p1.y - y) - (_p1.x - x) * (_p3.y - y);
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

bool Triangle::inter_update(const Ray & ray, RayHit &hit) {
    Vec3 d = ray.get_direction();
    Vec3 p = ray.get_origin();
    float t = d.dot(norm);
    if(std::fabs(t) < EPS) {
        return false;
    }
    t = ((_p1 - p).dot(norm)) / t;
    if(std::isnan(t) || t < 0 || t >= hit.get_dis()) {
        return false;
    }
    Vec3 inter = p + d * t;
    float x = inter.x, y = inter.y, z = inter.z;
    float s1, s2, s3, t1, t3, sum;
    switch(type) {
        case 2:
            s1 = (_p1.y - y) * (_p2.z - z) - (_p2.y - y) * (_p1.z - z);
            s2 = (_p2.y - y) * (_p3.z - z) - (_p3.y - y) * (_p2.z - z);
            s3 = (_p3.y - y) * (_p1.z - z) - (_p1.y - y) * (_p3.z - z);
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
            s1 = (_p1.x - x) * (_p2.z - z) - (_p2.x - x) * (_p1.z - z);
            s2 = (_p2.x - x) * (_p3.z - z) - (_p3.x - x) * (_p2.z - z);
            s3 = (_p3.x - x) * (_p1.z - z) - (_p1.x - x) * (_p3.z - z);
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
            s1 = (_p1.x - x) * (_p2.y - y) - (_p2.x - x) * (_p1.y - y);
            s2 = (_p2.x - x) * (_p3.y - y) - (_p3.x - x) * (_p2.y - y);
            s3 = (_p3.x - x) * (_p1.y - y) - (_p1.x - x) * (_p3.y - y);
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

bool Triangle::if_inter_dis(const Ray & ray, float dis) {
    Vec3 d = ray.get_direction();
    Vec3 p = ray.get_origin();
    float t = d.dot(norm);
    if(std::fabs(t) < EPS) {
        return false;
    }
    t = ((_p1 - p).dot(norm)) / t;
    if(std::isnan(t) || t < 0) {
        return false;
    }
    if(t >= dis) {
        return false;
    }
    Vec3 inter = p + d * t;
    float x = inter.x, y = inter.y, z = inter.z;
    float s1, s2, s3;
    switch(type) {
        case 2:
            s1 = (_p1.y - y) * (_p2.z - z) - (_p2.y - y) * (_p1.z - z);
            s2 = (_p2.y - y) * (_p3.z - z) - (_p3.y - y) * (_p2.z - z);
            s3 = (_p3.y - y) * (_p1.z - z) - (_p1.y - y) * (_p3.z - z);
            if((s1 < 0 && s2 < 0 && s3 < 0) || (s1 > 0 && s2 > 0 && s3 > 0)) {
                return true;
            }
            return false;
        break;
        case 1:
            s1 = (_p1.x - x) * (_p2.z - z) - (_p2.x - x) * (_p1.z - z);
            s2 = (_p2.x - x) * (_p3.z - z) - (_p3.x - x) * (_p2.z - z);
            s3 = (_p3.x - x) * (_p1.z - z) - (_p1.x - x) * (_p3.z - z);
            if((s1 < 0 && s2 < 0 && s3 < 0) || (s1 > 0 && s2 > 0 && s3 > 0)) {
                return true;
            }
            return false;
        break;
        case 0:
            s1 = (_p1.x - x) * (_p2.y - y) - (_p2.x - x) * (_p1.y - y);
            s2 = (_p2.x - x) * (_p3.y - y) - (_p3.x - x) * (_p2.y - y);
            s3 = (_p3.x - x) * (_p1.y - y) - (_p1.x - x) * (_p3.y - y);
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

Vec2 Triangle::inter_to_local(const Vec3 &inter) {
    float x = inter.x, y = inter.y, z = inter.z;
    float s1, s2, s3, sum, t1, t3;
    switch(type) {
        case 2:
            s1 = (_p1.y - y) * (_p2.z - z) - (_p2.y - y) * (_p1.z - z);
            s2 = (_p2.y - y) * (_p3.z - z) - (_p3.y - y) * (_p2.z - z);
            s3 = (_p3.y - y) * (_p1.z - z) - (_p1.y - y) * (_p3.z - z);
            sum = s1 + s2 + s3;
            t1 = s1 / sum;
            t3 = s3 / sum;
            return Vec2(t1, t3);
        break;
        case 1:
            s1 = (_p1.x - x) * (_p2.z - z) - (_p2.x - x) * (_p1.z - z);
            s2 = (_p2.x - x) * (_p3.z - z) - (_p3.x - x) * (_p2.z - z);
            s3 = (_p3.x - x) * (_p1.z - z) - (_p1.x - x) * (_p3.z - z);
            sum = s1 + s2 + s3;
            t1 = s1 / sum;
            t3 = s3 / sum;
            return Vec2(t1, t3);
        break;
        case 0:
            s1 = (_p1.x - x) * (_p2.y - y) - (_p2.x - x) * (_p1.y - y);
            s2 = (_p2.x - x) * (_p3.y - y) - (_p3.x - x) * (_p2.y - y);
            s3 = (_p3.x - x) * (_p1.y - y) - (_p1.x - x) * (_p3.y - y);
            sum = s1 + s2 + s3;
            t1 = s1 / sum;
            t3 = s3 / sum;
            return Vec2(t1, t3);
        break;
        default:
            assert(0);
    }
    return Vec2(0.0f, 0.0f);
}

void Triangle::trans(Mat3&T) {
    _p1 = T.map(_p1);
    _p2 = T.map(_p2);
    _p3 = T.map(_p3);
    norm = ((_p2 - _p1).cross(_p3 - _p1)).norm();
    float x1 = _p1.x, y1 = _p1.y;
    float x2 = _p2.x, y2 = _p2.y;
    float x3 = _p3.x, y3 = _p3.y;
    float dx12 = x2 - x1, dy12 = y2 - y1;
    float dx13 = x3 - x1, dy13 = y3 - y1;
    bool small_xy = std::fabs(dx12 * dy13 - dx13 * dy12) < EPS;
    float z1 = _p1.z;
    float z2 = _p2.z;
    float z3 = _p3.z;
    float dz12 = z2 - z1;
    float dz13 = z3 - z1;
    bool small_xz = std::fabs(dx12 * dz13 - dx13 * dz12) < EPS;
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
    _area = (_p2 - _p1).cross(_p3 - _p1).len() / 2.0f;
}

void Triangle::debug() {
    std::cerr << "Debug Triangle: ";
    std::cerr << '(' << _p1.x << ',' << _p1.y << ',' << _p1.z << ')';
    std::cerr << '(' << _p2.x << ',' << _p2.y << ',' << _p2.z << ')';
    std::cerr << '(' << _p3.x << ',' << _p3.y << ',' << _p3.z << ')' << std::endl;
    std::cerr << "          Norm: " << norm.x << ' ' << norm.y << ' ' << norm.z << std::endl;
}

Box Triangle::bound() {
    Box b(_p1, _p2);
    b.add(_p3);
    return b;
}

float Triangle::area() {
    return _area;
}

const Vec3 Triangle::get_norm() const {
    return norm;
}

const Vec3 Triangle::get_p1() const {
    return _p1;
}

const Vec3 Triangle::get_p2() const {
    return _p2;
}

const Vec3 Triangle::get_p3() const {
    return _p3;
}

const Vec3 Triangle::interpolate_norm(const Vec3 &n1, const Vec3 &n2, const Vec3 &n3, const Vec2 &local) {
    float c1 = local.x;
    float c3 = local.y;
    float c2 = 1.0f - c1 - c3;
    return n3 * c1 + n1 * c2 + n2 * c3;
}

const UV Triangle::interpolate_uv(const UV &u1, const UV &u2, const UV &u3, const Vec2 &local) {
    float c1 = local.x;
    float c3 = local.y;
    float c2 = 1.0f - c1 - c3;
    return UV(u3.x * c1 + u1.x * c2 + u2.x * c3,
              u3.y * c1 + u1.y * c2 + u2.y * c3);
}