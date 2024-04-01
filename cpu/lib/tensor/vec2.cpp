#include <cmath>
#include "vec2.hpp"
#include "../const.hpp"

Vec2::Vec2(): x(0.0f), y(0.0f) {
}

Vec2::~Vec2() {
}

Vec2::Vec2(float nx, float ny): x(nx), y(ny) {
}

Vec2 Vec2::operator+(const Vec2& b) const {
    return Vec2(x + b.x, y + b.y);
}

Vec2 Vec2::operator-(const Vec2 &b) const {
    return Vec2(x - b.x, y - b.y);
}

Vec2 Vec2::operator*(const float d) const {
    return Vec2(x * d, y * d);
}

/// unsafe operator
Vec2 Vec2::operator/(const float d) const {
    return Vec2(x / d, y / d);
}

void Vec2::operator+=(const Vec2 &b) {
    x += b.x, y += b.y;
}

void Vec2::operator-=(const Vec2 &b) {
    x -= b.x, y -= b.y;
}

void Vec2::operator*=(const float d) {
    x *= d, y *= d;
}

/// unsafe operator
void Vec2::operator/=(const float d) {
    x /= d, y /= d;
}

void Vec2::operator*=(const Vec2 &b) {
    x *= b.x, y *= b.y;
}

void Vec2::operator/=(const Vec2 &b) {
    x /= b.x, y /= b.y;
}

Vec2 Vec2::operator*(const Vec2 &b) {
    return Vec2(x * b.x, y * b.y);
}

float Vec2::sum() {
    return x + y;
}

float Vec2::dot(const Vec2 &b) {
    return x * b.x + y * b.y;
}

Vec2 Vec2::norm() {
    float s = sqrt(x * x + y * y);
    return Vec2(x / s, y / s);
}

void Vec2::normalize() {
    float s = sqrt(x * x + y * y);
    x /= s;
    y /= s;
}

float Vec2::len() {
    return sqrt(x * x + y * y);
}

float Vec2::angle(const Vec2 &b) {
    float s = (x * b.x + y * b.y) / 
                std::sqrt((x * x + y * y) * (b.x * b.x + b.y * b.y));
    if(s <= -1.0) {
        return PI;
    }
    if(s >= 1.0) {
        return 0;
    }
    return acos(s);
}

float Vec2::square() {
    return x * x + y * y;
}
