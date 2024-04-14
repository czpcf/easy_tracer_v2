#include "camera_perspective.hpp"

#include<iostream>
using namespace std;

CameraPerspective::CameraPerspective(const Vec3 &eye, const Vec3 &direction, const Vec3 &up, int w, int h, float angle):
    Camera(eye, direction, up, w, h) {
    // make sure the vectors are normalized
    cx = float(width) / 2.0f;
    cy = float(height) / 2.0f;
    fx = std::tan(angle / 2.0f) / cx * cx / cy;
    fy = std::tan(angle / 2.0f) / cy;
    T = Mat3::look_at(this->eye, this->eye + this->direction, this->up);
    T_inv = Mat3::look_at_inv(this->eye, this->eye + this->direction, this->up);
}

Ray CameraPerspective::generate_ray(const Vec2 &uv) {
    Vec3 p(0.0f, 0.0f, 0.0f);
    Vec3 d((cx - uv.x) * fx, (uv.y - cy) * fy, 1);
    d.normalize();
    Vec3 np = T.map(p);
    Vec3 nd = T.map_scale(d);
    return Ray(np, nd);
}

void CameraPerspective::sample_point(int x, int y, RNG *rng, Vec3 &p_on_film, float &pdf, Vec2 &local) {
    p_on_film = eye;
    local = Vec2(x, y);
    pdf = -1.0f;
}

void CameraPerspective::sample_direction(RNG *rng, const Vec2 &local, Vec3 &dir, float &pdf) {
    float dx = rng->rand_float();
    float dy = rng->rand_float();
    Vec3 d((cx - local.x + dx) * fx, (local.y - cy + dy) * fy, 1);
    
    // float jacobian = d.square();
    // jacobian = jacobian * jacobian;
    
    d.normalize();
    Vec3 nd = T.map_scale(d);
    dir = nd;
    // pdf = jacobian;
    pdf = 1.0f;
}

void CameraPerspective::sample_ray(int x, int y, RNG *rng, Ray &ray, float &pdf) {
    float dx = rng->rand_float();
    float dy = rng->rand_float();
    Vec3 d((cx - x + dx) * fx, (y - cy + dy) * fy, 1);
    
    // float jacobian = d.square();
    // jacobian = jacobian * jacobian;
    
    d.normalize();
    Vec3 nd = T.map_scale(d);
    ray = Ray(eye, nd);
    // pdf = jacobian;
    pdf = 1.0f;
}

/// TODO: maybe pdf is useless ?
bool CameraPerspective::sample_in_ray(int x, int y, RNG *rng, const Vec3 &inter, Ray &ray, float &pdf) {
    Vec3 dir = (inter - eye).norm();
    Vec3 ndir = T_inv.map_scale(dir);
    if(ndir.z < 1e-6) {
        return false;
    }
    ndir /= ndir.z;
    float gx = -(ndir.x / fx - cx - 1.0f);
    float gy = ndir.y / fy + cy;
    // the very wicked bias
    if((float(x) - gx > 1e-4 || gx - 1.0f - x > 1e-4) ||
       (float(y) - gy > 1e-4 || gy - 1.0f - y > 1e-4)) {
        return false;
    }
    ray = Ray(inter, dir);
    pdf = -1.0f;
}

float CameraPerspective::pdf_point(const Vec3 &p_in_film) {
    return -1.0f;
}

float CameraPerspective::pdf_ray(const Ray &ray_out) {
    return 1.0f;
    // Vec3 ndir = T_inv.map_scale(ray_out.get_direction());
    // ndir /= ndir.z;
    // float jacobian = ndir.square();
    // return jacobian * jacobian;
}

const AreaType CameraPerspective::area_type() const {
    return AreaType::ZERO;
}

const DirType CameraPerspective::dir_type() const {
    return DirType::CONTINOUS;
}
