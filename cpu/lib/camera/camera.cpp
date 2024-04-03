#include "camera.hpp"

Camera::Camera(const Vec3 &eye, const Vec3 &direction, const Vec3 &up, int w, int h) {
    this->eye = eye;

    this->direction = direction;
    this->direction.normalize();
    
    this->horizontal = this->direction.cross(up);
    this->horizontal.normalize();

    this->up = this->horizontal.cross(this->direction);
    this->up.normalize();
    
    this->up = up;
    this->width = w;
    this->height = h;
}

int Camera::get_width() {
    return width;
}

int Camera::get_height() {
    return height;
}