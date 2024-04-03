/*
camera
*/
#ifndef CAMERA_H
#define CAMERA_H

#include "../tensor/mat3.hpp"
#include "../tensor/vec2.hpp"
#include "../tensor/vec3.hpp"
#include "../ray/ray.hpp"

class Camera {
public:
    Camera(const Vec3 &eye, const Vec3 &direction, const Vec3 &up, int w, int h);
    virtual ~Camera() = default;

    /// @brief return width
    int get_width();

    /// @brief return height
    int get_height();

    /// @brief generate ray
    virtual Ray generate_ray(const Vec2 &) = 0;

    /// @brief MIS needs this
    virtual bool inside_pixel(const Vec3 &from, const Vec3 &to, float &pdf) = 0;

protected:
    Vec3 eye;
    Vec3 direction;
    Vec3 up;
    Vec3 horizontal;
    int width;
    int height;
};
#endif