/*
bxdf
*/
#ifndef BXDF_H
#define BXDF_H

#include "../tensor/vec2.hpp"
#include "../tensor/vec3.hpp"
#include "../tensor/mat3.hpp"
#include "../const.hpp"

class Surface;
class Sampler;

class Bxdf {
public:
    Bxdf();
    virtual ~Bxdf() = default;

    /// @brief return phase function(in, out, normal),
    /// in-direction is always from surface to light,
    /// out-direction is always from surface to eye
    virtual Vec3 phase(const Surface &surface, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm);
};

class Surface {
public:
    Surface();
    ~Surface() = default;
    Surface(const Vec3 &_normal, const Vec3 &_x, const Vec3 &_y, const Vec3 &_color, Bxdf *b, Sampler *sampler);

    /// @brief return bxdf
    Bxdf *get_bxdf();

    /// @brief return sampler
    Sampler *get_sampler();

    /// @brief return surface normal
    const Vec3 get_normal() const;

    /// @brief return x-axis
    const Vec3 get_x() const;

    /// @brief return y-axis
    const Vec3 get_y() const;

    /// @brief return surface color,
    /// ATTENTION: its ONLY used in bxdf
    const Vec3 get_color() const;

    /// @brief return cos<w, normal>
    const float cos_theta(const Vec3 &w) const;

    /// @brief return cos of phi
    const float cos_phi(const Vec3 &w) const;

    /// @brief world to local
    const Vec3 world_to_local(const Vec3 &w) const;

    /// @brief local to world
    const Vec3 local_to_world(const Vec3 &w) const;

private:
    Vec3 normal;
    Vec3 x;
    Vec3 y;
    Vec3 color;
    Bxdf *bxdf;
    Sampler *sampler;
};

#endif