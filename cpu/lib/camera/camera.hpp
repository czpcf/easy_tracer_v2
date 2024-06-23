/*
cameras
*/
#ifndef CAMERA_H
#define CAMERA_H

#include "../tensor/vec2.hpp"
#include "../tensor/vec3.hpp"
#include "../tensor/mat3.hpp"
#include "../const.hpp"
#include "../random/random.hpp"
#include "../bxdf/bxdf.hpp"
#include "../utils.hpp"
#include "../ray/ray.hpp"

// this sampler class ONLY supports pure continuous pdf or pure disctrete pdf,
// cannot combine them together
class Camera {
public:
    Camera(const Vec3 &eye, const Vec3 &direction, const Vec3 &up, int w, int h);
    virtual ~Camera() = default;
    
    /// @brief return width
    int get_width();

    /// @brief return height
    int get_height();
    
    /// @brief generate ray regardless of its pdf
    virtual Ray generate_ray(RNG *rng, const Vec2 &) = 0;

    /// @brief sample point in the pixel (x, y) on the film given the intersection on the shape, and set pdf,
    /// if pdf < 0, its delta distribution
    virtual void sample_point(int x, int y, RNG *rng, Vec3 &p_on_film, float &pdf, Vec2 &local) = 0;

    /// @brief sample direction and set pdf,
    /// if pdf < 0, its delta distribution
    virtual void sample_direction(RNG *rng, const Vec2 &local, Vec3 &dir, float &pdf) = 0;

    /// @brief sample ray in the pixel (x, y) on the film and set pdf,
    /// if pdf < 0, its delta distribution
    virtual void sample_ray(int x, int y, RNG *rng, Ray &ray, float &pdf) = 0;

    /// @brief sample ray on the film and set pdf according to the intersection on the shape,
    /// the ray should point at the point on the surface,
    /// NOTICE: direction is from FILM to SURFACE,
    /// if return false, cannont sample or its NOT in pixel (x, y),
    /// if pdf < 0, its delta distribution
    virtual bool sample_in_ray(int x, int y, RNG *rng, const Vec3 &inter, Ray &ray, float &pdf) = 0;

    /// @brief set pdf according to the point on the film,
    /// ALWAYS assuming the ray is ON the film,
    /// if pdf < 0, its delta distribution
    virtual float pdf_point(const Vec3 &p_on_film) = 0;

    /// @brief set pdf according to the ray from film -> surface,
    /// ALWAYS assuming the ray is ON the film
    virtual float pdf_ray(const Ray &ray_out) = 0;

    /// @brief return area type
    const virtual AreaType area_type() const = 0;

    /// @brief return direction type
    const virtual DirType dir_type() const = 0;

protected:
    Vec3 eye;
    Vec3 direction;
    Vec3 up;
    Vec3 horizontal;
    int width;
    int height;
};
#endif