#include <cmath>
#include <assert.h>
#include "vec3.hpp"
#include "../const.hpp"


static float _min(float x, float y) {
    return x < y ? x : y;
}

static float _max(float x, float y) {
    return x > y ? x : y;
}


Vec3::Vec3(): x(0.0f), y(0.0f), z(0.0f) {
}

Vec3::~Vec3() {
}

Vec3::Vec3(float nx, float ny, float nz): x(nx), y(ny), z(nz) {
}

Vec3 Vec3::operator+(const Vec3 &b) const {
    return Vec3(x + b.x, y + b.y, z + b.z);
}

Vec3 Vec3::operator-() const {
    return Vec3(-x, -y, -z);
}

Vec3 Vec3::operator-(const Vec3 &b) const {
    return Vec3(x - b.x, y - b.y, z - b.z);
}

Vec3 Vec3::operator*(const float d) const {
    return Vec3(x * d, y * d, z * d);
}

/// unsafe operator
Vec3 Vec3::operator/(const float d) const {
    return Vec3(x / d, y / d, z / d);
}

void Vec3::operator+=(const Vec3 &b) {
    x += b.x, y += b.y, z += b.z;
}

void Vec3::operator-=(const Vec3 &b) {
    x -= b.x, y -= b.y, z -= b.z;
}

void Vec3::operator*=(const float d) {
    x *= d, y *= d, z *=d;
}

/// unsafe operator
void Vec3::operator/=(const float d) {
    x /= d, y /= d, z /=d;
}

void Vec3::operator*=(const Vec3 &b) {
    x *= b.x, y *= b.y, z *= b.z;
}

void Vec3::operator/=(const Vec3 &b) {
    x /= b.x, y /= b.y, z /= b.z;
}
/// return the cross product of a and b
Vec3 Vec3::cross(const Vec3&b) {
    return Vec3(y * b.z - z * b.y,
                z * b.x - x * b.z,
                x * b.y - y * b.x);
}

Vec3 Vec3::operator*(const Vec3 &b) {
    return Vec3(x * b.x, y * b.y, z * b.z);
}

float Vec3::sum() {
    return x + y + z;
}

float Vec3::dot(const Vec3 &b) {
    return x * b.x + y * b.y + z * b.z;
}

Vec3 Vec3::norm() {
    float s = std::sqrt(x * x + y * y + z * z);
    return Vec3(x / s, y / s, z / s);
}

void Vec3::normalize() {
    float s = std::sqrt(x * x + y * y + z * z);
    x /= s;
    y /= s;
    z /= s;
}

float Vec3::len() {
    return std::sqrt(x * x + y * y + z * z);
}

float Vec3::angle(const Vec3 &b) {
    float s = (x * b.x + y * b.y + z * b.z) / 
                std::sqrt((x * x + y * y + z * z) * (b.x * b.x + b.y * b.y + b.z * b.z));
    if(s <= -1.0) {
        return PI;
    }
    if(s >= 1.0) {
        return 0;
    }
    return acos(s);
}

float Vec3::square() {
    return x * x + y * y + z * z;
}

Vec3 Vec3::reflect(const Vec3 &b) {
    float nx = b.x, ny = b.y, nz = b.z;
    // see implementation of mat3
    // a special case of ratating around an axis
    return Vec3((nx * nx * 2 - 1) * x + (nx * ny * 2) * y + (nx * nz * 2) * z,
               (ny * nx * 2) * x + (ny * ny * 2 - 1) * y + (ny * nz * 2) * z,
               (nz * nx * 2) * x + (nz * ny * 2) * y + (nz * nz * 2 - 1) * z);
}

Vec3 Vec3::abs() {
    return Vec3(std::abs(x), std::abs(y), std::abs(z));
}

float Vec3::max() {
    return _max(_max(x, y), z);
}

float Vec3::min() {
    return _min(_min(x, y), z);
}


Vec3 CrossSafe(Vec3 a, Vec3 b) {
    Vec3 c = a.cross(b);
    if(c.abs().max() > 1e-6) {
        return c;
    }
    if(std::abs(a.x) >= 1e-6) {
        c.x = (a.y + a.z) / a.x;
        c.y = -1.0f;
        c.z = -1.0f;
    } else if(std::abs(a.y) >= 1e-6) {
        c.x = -1.0f;
        c.y = (a.x + a.z) / a.y;
        c.z = -1.0f;
    } else {
        c.x = -1.0f;
        c.y = -1.0f;
        c.z = (a.x + a.y) / a.z;
    }
    return c;
}

Vec3 min(Vec3 a, Vec3 b) {
    return Vec3(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z);
}

Vec3 max(Vec3 a, Vec3 b) {
    return Vec3(a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z);
}
