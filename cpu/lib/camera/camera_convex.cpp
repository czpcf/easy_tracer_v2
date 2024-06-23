#include "camera_convex.hpp"
#include <iostream>
#include <assert.h>

CameraConvex::CameraConvex(
    const Vec3 &eye, const Vec3 &direction, const Vec3 &up, 
    int w, int h,
    float aperture, float focus, float dis, float scale):
    Camera(eye, direction, up, w, h) {
    // make sure the vectors are normalized
    cx = float(width) / 2.0f;
    cy = float(height) / 2.0f;

    A = aperture;
    F = focus;
    D = dis;
    if(F > D) {
        fprintf(stderr, "Camera's F is greater than D!");
        exit(1);
    }
    H = 1.0f / (1.0f / F - 1.0f / D);
    this->scale = scale;
    mag = H / D;
    sx = (float)w / h;
    sy = 1.0f;

    T = Mat3::look_at(this->eye, this->eye + this->direction, this->up);
    T_inv = Mat3::look_at_inv(this->eye, this->eye + this->direction, this->up);
}

Ray CameraConvex::generate_ray(RNG *rng, const Vec2 &uv) {
    Vec2 uv2((cx - uv.x) / width * 2.0f, (uv.y - cy) / height * 2.0f);
    float r = rng->rand_float() * A;
    float theta = PI * 2.0f * rng->rand_float();
    r = 0;
    float nx = sinf(theta) * r;
    float ny = cosf(theta) * r;
    Vec3 p(nx, ny, 0.0f);
    Vec3 img(-uv2.x * scale * mag * sx, -uv2.y * scale * mag * sy, H);
    Vec3 d = (img - p).norm();
    Vec3 np = T.map(p);
    Vec3 nd = T.map_scale(d).norm();
    return Ray(np, nd);
}

/// TODO: not implemented
void CameraConvex::sample_point(int x, int y, RNG *rng, Vec3 &p_on_film, float &pdf, Vec2 &local) {
    fprintf(stderr, "CameraConvex::sample_in_ray not implemented yet!\n");
    assert(0);
}

/// TODO: not implemented
void CameraConvex::sample_direction(RNG *rng, const Vec2 &local, Vec3 &dir, float &pdf) {
    fprintf(stderr, "CameraConvex::sample_direction not implemented yet!\n");
    assert(0);
}

void CameraConvex::sample_ray(int x, int y, RNG *rng, Ray &ray, float &pdf) {
    float dx = rng->rand_float();
    float dy = rng->rand_float();
    Vec2 uv((float(x) - cx + dx) / width * 2.0f, (cy - float(y) + float(dy)) / height * 2.0f);

    float r = rng->rand_float() * A;
    float theta = PI * 2.0f * rng->rand_float();
    float nx = sinf(theta) * r;
    float ny = cosf(theta) * r;
    Vec3 p(nx, ny, 0.0f);
    Vec3 img(-uv.x * scale * mag * sx, -uv.y * scale * mag * sy, H);
    Vec3 d = (img - p).norm();

    Vec3 np = T.map(p);
    Vec3 nd = T.map_scale(d).norm();
    ray = Ray(np, nd);
    pdf = 1.0f;
}

/// TODO: not implemented
bool CameraConvex::sample_in_ray(int x, int y, RNG *rng, const Vec3 &inter, Ray &ray, float &pdf) {
    fprintf(stderr, "CameraConvex::sample_in_ray not implemented yet!\n");
    assert(0);
    return true;
}

/// TODO: not implemented
float CameraConvex::pdf_point(const Vec3 &p_in_film) {
    fprintf(stderr, "CameraConvex::pdf_point not implemented yet!\n");
    assert(0);
    return -1.0f;
}

/// TODO: not implemented
float CameraConvex::pdf_ray(const Ray &ray_out) {
    fprintf(stderr, "CameraConvex::pdf_ray not implemented yet!\n");
    assert(0);
    return 1.0f;
}

const AreaType CameraConvex::area_type() const {
    return AreaType::FINITE;
}

const DirType CameraConvex::dir_type() const {
    return DirType::CONTINOUS;
}
