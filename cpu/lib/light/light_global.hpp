/*
global light
a big ball containg the scene
*/

#ifndef LIGHT_GLOBAL_H
#define LIGHT_GLOBAL_H

#include "light.hpp"
#include "../geometry/point.hpp"

class LightGlobal: public Light {
public:
    LightGlobal();
    ~LightGlobal() override = default;

    /// @brief sample point on the light given the point on the surface, and set pdf
    /// if pdf < 0, its delta distribution
    void sample_point(RNG *rng, const Vec3 &p_on_suf, Vec3 &p_on_light, float &pdf, Vec2 &local) override;

    /// @brief sample direction and set pdf
    /// if pdf < 0, its delta distribution
    void sample_direction(RNG *rng, const Vec3 &p_on_suf, Vec3 &dir, float &pdf) override;

    /// @brief sample ray on the light and set pdf
    /// if pdf < 0, its delta distribution
    void sample_ray(RNG *rng, Ray &ray, float &pdf, Vec2 &local) override;

    /// @brief sample ray on the light and set pdf according to the point on the surface
    /// the ray should point at the point on the surface,
    /// if return false, cannont sample,
    /// if pdf < 0, its delta distribution
    bool sample_in_ray(RNG *rng, const Vec3 &p_on_suf, Ray &ray, float &pdf, Vec2 &local) override;

    /// @brief set pdf according to the point on the light
    /// ALWAYS assuming the ray is ON the light
    /// if pdf < 0, its delta distribution
    float pdf_point(const Vec3 &) override;

    /// @brief set pdf according to the ray from light -> surface
    /// ALWAYS assuming the ray is ON the light
    float pdf_ray(const Ray &ray_out) override;

    /// @brief return shape
    Geometry *get_shape() override;

    /// @brief is this light specular(cannot sample on its manifold) ?
    bool is_specular() override;

    /// @brief decaying of light
    float decaying(const Vec3 &p_on_suf, const Vec3 &p_on_light) override;

private:
    Point p;
};

#endif