/*
point light
*/

#ifndef LIGHT_POINT_H
#define LIGHT_POINT_H

#include "light.hpp"
#include "../geometry/point.hpp"

class LightPoint: public Light {
public:
    LightPoint();
    LightPoint(const Point &p);
    ~LightPoint() override = default;

    /// @brief can sample point on the light?
    bool can_sample_point() override;

    /// @brief sample point on the light given the point on the surface, and set pdf
    /// if pdf < 0, its delta distribution
    void sample_point(RNG *rng, const Vec3 &p_on_suf, Vec3 &p_on_light, float &pdf) override;

    /// @brief sample direction and set pdf
    /// if pdf < 0, its delta distribution
    void sample_direction(RNG *rng, const Vec3 &p_on_suf, Vec3 &dir, float &pdf) override;

    /// @brief sample ray on the light and set pdf
    /// if pdf < 0, its delta distribution
    void sample_ray(RNG *rng, Ray &ray, float &pdf) override;

    /// @brief sample ray on the light and set pdf according to the point on the surface
    /// the ray should point at the point on the surface
    /// if pdf < 0, its delta distribution
    void sample_in_ray(RNG *rng, const Vec3 &p_on_suf, Ray &ray, float &pdf) override;

    /// @brief set pdf according to the point on the light
    /// ALWAYS assuming the ray is ON the light
    /// if pdf < 0, its delta distribution
    float pdf_point(const Vec3 &) override;

    /// @brief set pdf according to the ray from light -> surface
    /// ALWAYS assuming the ray is ON the light
    float pdf_ray(const Ray &ray_out) override;

    /// @brief return shape
    Geometry *get_shape() override;

private:
    Point o;
};

#endif