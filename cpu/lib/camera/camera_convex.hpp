#ifndef CAMERA_CONVEX_H
#define CAMERA_CONVEX_H

#include "camera.hpp"
#include <cmath>

class CameraConvex: public Camera {
public:
    CameraConvex(const Vec3 &eye, const Vec3 &direction, const Vec3 &up, 
        int w, int h,
        float aperture, float focus, float dis, float scale);
    virtual ~CameraConvex() = default;
    
    /// @brief return width
    int get_width();

    /// @brief return height
    int get_height();
    
    /// @brief generate ray regardless of its pdf
    Ray generate_ray(RNG *rng, const Vec2 &) override;

    /// @brief sample point on the film given the intersection on the shape, and set pdf,
    /// if pdf < 0, its delta distribution
    void sample_point(int x, int y, RNG *rng, Vec3 &p_on_film, float &pdf, Vec2 &local) override;

    /// @brief sample direction and set pdf,
    /// if pdf < 0, its delta distribution
    void sample_direction(RNG *rng, const Vec2 &local, Vec3 &dir, float &pdf) override;

    /// @brief sample ray on the light and set pdf,
    /// if pdf < 0, its delta distribution
    void sample_ray(int x, int y, RNG *rng, Ray &ray, float &pdf) override;

    /// @brief sample ray on the film and set pdf according to the intersection on the shape,
    /// the ray should point at the point on the surface,
    /// NOTICE: direction is from FILM to SURFACE,
    /// if return false, cannont sample or its NOT in pixel (x, y),
    /// if pdf < 0, its delta distribution
    bool sample_in_ray(int x, int y, RNG *rng, const Vec3 &inter, Ray &ray, float &pdf) override;

    /// @brief set pdf according to the point on the film,
    /// ALWAYS assuming the ray is ON the film,
    /// if pdf < 0, its delta distribution
    float pdf_point(const Vec3 &p_on_film) override;

    /// @brief set pdf according to the ray from film -> surface,
    /// ALWAYS assuming the ray is ON the film
    float pdf_ray(const Ray &ray_out) override;

    /// @brief return area type
    const AreaType area_type() const override;

    /// @brief return direction type
    const DirType dir_type() const override;

private:
    /// @brief aperture
    float A;
    /// @brief focus
    float F;
    /// @brief distance between the film and the lens
    float D;
    /// @brief 1/f = 1/d + 1/h
    float H;
    /// @brief scaling the film
    float scale;
    /// @brief magnitude = H/D
    float mag;
    float cx;
    float cy;
    float sx;
    float sy;
    Mat3 T;
    Mat3 T_inv;
};

#endif