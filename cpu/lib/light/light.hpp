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

    /// @brief sample point on the light given the intersection on the shape, and set pdf,
    /// if pdf < 0, its delta distribution
    virtual void sample_point(RNG *rng, const Vec3 &inter, Vec3 &p_on_light, float &pdf, Vec2 &local) = 0;

    /// @brief sample direction and set pdf,
    /// if pdf < 0, its delta distribution
    virtual void sample_direction(RNG *rng, const Vec3 &p_on_suf, Vec3 &dir, float &pdf) = 0;

    /// @brief sample ray on the light and set pdf,
    /// if pdf < 0, its delta distribution
    virtual void sample_ray(RNG *rng, Ray &ray, float &pdf, Vec2 &local) = 0;

    /// @brief sample ray on the light and set pdf according to the intersection on the shape,
    /// the ray should point at the point on the surface,
    /// NOTICE: direction is from LIGHT to SURFACE,
    /// if return false, cannont sample,
    /// if pdf < 0, its delta distribution
    virtual bool sample_in_ray(RNG *rng, const Vec3 &inter, Ray &ray, float &pdf, Vec2 &local) = 0;

    /// @brief set pdf according to the point on the light,
    /// ALWAYS assuming the ray is ON the light,
    /// if pdf < 0, its delta distribution
    virtual float pdf_point(const Vec3 &p_on_light) = 0;

    /// @brief set pdf according to the ray from light -> surface,
    /// ALWAYS assuming the ray is ON the light
    virtual float pdf_ray(const Ray &ray_out) = 0;

    /// @brief return shape of the light
    virtual Geometry *get_shape() = 0;

    /// @brief is this light specular(cannot sample on its manifold) ?
    virtual bool is_specular() = 0;

    /// @brief decaying of light
    virtual float decaying(const Vec3 &p_on_suf, const Vec3 &p_on_light) = 0;
};

#endif