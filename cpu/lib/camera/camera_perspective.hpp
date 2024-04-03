#ifndef CAMERA_PERSPECTIVE_H
#define CAMERA_PERSPECTIVE_H

#include "camera.hpp"
#include <cmath>

class CameraPerspective: public Camera {
public:
    CameraPerspective(const Vec3 &eye, const Vec3 &direction, const Vec3 &up, int w, int h, float angle);

    Ray generate_ray(const Vec2 &) override;

    bool inside_pixel(const Vec3 &from, const Vec3 &to, float &pdf) override;

private:
    float cx;
    float cy;
    float fx;
    float fy;
};

#endif