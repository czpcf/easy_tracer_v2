/*
ray, should guarantee dir is always normalized upon construction
*/
#ifndef RAY_H
#define RAY_H
#include "../tensor/vec3.hpp"

class Ray {
public:
    Ray();
    ~Ray() = default;

    /// @brief p = origin, d = dir
    Ray(Vec3 origin, Vec3 dir);

    const Vec3 &get_origin() const;

    const Vec3 &get_direction() const;

private:
    Vec3 p;
    Vec3 d;
};
#endif
