/*
light
support sampling method and geometry
*/

#ifndef LIGHT_H
#define LIGHT_H

#include "../tensor/vec2.hpp"
#include "../tensor/vec3.hpp"
#include "../random/random.hpp"
#include "../ray/ray.hpp"
#include "../geometry/geometry.hpp"

class Light {
public:
    Light();
    virtual ~Light() = default;

    /// @brief can sample point on the light?
    virtual bool can_sample_point() = 0;

    /// @brief sample point on the light given the point on the surface, and set pdf
    /// if pdf < 0, its delta distribution
    virtual void sample_point(RNG *, const Vec3 &, Vec3 &, float &) = 0;

    /// @brief sample direction and set pdf
    /// if pdf < 0, its delta distribution
    virtual void sample_direction(RNG *, const Vec3 &, Vec3 &, float &) = 0;

    /// @brief sample ray on the light and set pdf
    /// if pdf < 0, its delta distribution
    virtual void sample_ray(RNG *, Ray &ray, float &pdf) = 0;

    /// @brief sample ray on the light and set pdf according to the point on the surface
    /// the ray should point at the point on the surface
    /// if pdf < 0, its delta distribution
    virtual void sample_in_ray(RNG *, const Vec3 &, Ray &ray, float &pdf) = 0;

    /// @brief set pdf according to the point on the light
    /// ALWAYS assuming the ray is ON the light
    /// if pdf < 0, its delta distribution
    virtual float pdf_point(const Vec3 &) = 0;

    /// @brief set pdf according to the ray from light -> surface
    /// ALWAYS assuming the ray is ON the light
    virtual float pdf_ray(const Ray &ray_out) = 0;

    /// @brief return shape of the light
    virtual Geometry *get_shape() = 0;
};

#endif