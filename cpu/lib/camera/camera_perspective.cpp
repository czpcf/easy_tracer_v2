#include "camera_perspective.hpp"

#include<iostream>
using namespace std;

CameraPerspective::CameraPerspective(const Vec3 &eye, const Vec3 &direction, const Vec3 &up, int w, int h, float angle):
    Camera(eye, direction, up, w, h) {
    // make sure the vectors are normalized
    cx = float(width) / 2.0f;
    cy = float(height) / 2.0f;
    fx = std::tan(angle / 2.0f) / cx;
    fy = std::tan(angle / 2.0f) / cy;
}

Ray CameraPerspective::generate_ray(const Vec2 &uv) {
    Vec3 p(0.0f, 0.0f, 0.0f);
    Vec3 d((cx - uv.x) * fx, (uv.y - cy) * fy, 1);
    d.normalize();
    Mat3 T = Mat3::look_at(eye, eye + direction, up);
    Vec3 np = T.map(p);
    Vec3 nd = T.map_scale(d);
    return Ray(np, nd);
}

bool CameraPerspective::inside_pixel(const Vec3 &from, const Vec3 &to, float &pdf) {
    return false;
}