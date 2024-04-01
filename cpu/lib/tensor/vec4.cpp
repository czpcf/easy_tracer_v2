#include <cmath>
#include "vec4.hpp"
#include "../const.hpp"

Vec4::Vec4(): x(0.0f), y(0.0f), z(0.0f), w(0.0f) {
}

Vec4::~Vec4() {
}

Vec4::Vec4(float nx, float ny, float nz, float nw): x(nx), y(ny), z(nz), w(nw) {
}

Vec4 Vec4::operator+(const Vec4 &b) const {
    return Vec4(x + b.x, y + b.y, z + b.z, w + b.w);
}

Vec4 Vec4::operator-(const Vec4 &b) const {
    return Vec4(x - b.x, y - b.y, z - b.z, w - b.w);
}

Vec4 Vec4::operator*(const float d) const {
    return Vec4(x * d, y * d, z * d, w * d);
}

/// unsafe operator
Vec4 Vec4::operator/(const float d) const {
    return Vec4(x / d, y / d, z / d, w / d);
}

void Vec4::operator+=(const Vec4 &b) {
    x += b.x, y += b.y, z += b.z, w += b.w;
}

void Vec4::operator-=(const Vec4 &b) {
    x -= b.x, y -= b.y, z -= b.z, w -= b.w;
}

void Vec4::operator*=(const float d) {
    x *= d, y *= d, z *= d, w *= d;
}

/// unsafe operator
void Vec4::operator/=(const float d) {
    x /= d, y /= d, z /= d, w /= d;
}

void Vec4::operator*=(const Vec4 &b) {
    x *= b.x, y *= b.y, z *= b.z, w *= b.w;
}

void Vec4::operator/=(const Vec4 &b) {
    x /= b.x, y /= b.y, z /= b.z, w /= b.w;
}

Vec4 Vec4::operator*(const Vec4&b) {
    return Vec4(x * b.x, y * b.y, z * b.z, w * b.w);
}

float Vec4::sum() {
    return x + y + z + w;
}

float Vec4::dot(const Vec4&b) {
    return x * b.x + y * b.y + z * b.z + w * b.w;
}

Vec4 Vec4::norm() {
    float s = sqrt(x * x + y * y + z * z + w * w);
    return Vec4(x / s, y / s, z / s, w / s);
}

void Vec4::normalize() {
    float s = sqrt(x * x + y * y + z * z + w * w);
    x /= s;
    y /= s;
    z /= s;
    w /= w;
}

float Vec4::len() {
    return sqrt(x * x + y * y + z * z + w * w);
}

float Vec4::angle(const Vec4&b) {
    float s = (x * b.x + y * b.y + z * b.z + w * b.w) /
                sqrt((x * x + y * y + z * z + w * w) * (b.x * b.x + b.y * b.y + b.z * b.z + b.w * b.w));
    if(s <= -1.0) {
        return PI;
    }
    if(s >= 1.0) {
        return 0;
    }
    return acos(s);
}

float Vec4::square() {
    return x * x + y * y + z * z + w * w;
}

Vec3 Vec4::xyz() {
    return Vec3(x, y, z);
}
