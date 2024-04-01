/*
sampler for shapes
all the directions SHOULD point out of the surface
light -> eye is the forward direction
pdf will be set to -1 if specular
*/
#ifndef SAMPLER_H
#define SAMPLER_H

#include "../tensor/vec2.hpp"
#include "../tensor/vec3.hpp"
#include "../tensor/mat3.hpp"
#include "../const.hpp"
#include "../random/random.hpp"
#include "onb.hpp"

class Sampler {
public:
    Sampler();
    virtual ~Sampler() = default;

    /// @brief given dir_in and normal, set dir_out and pdf
    virtual void sample_out(RNG *rng, const Vec3 &dir_in, const Vec3 &norm, Vec3 &dir_out, float &pdf);

    /// @brief given dir_out and normal, set dir_in and pdf
    virtual void sample_in(RNG *rng, const Vec3 &dir_out, const Vec3 &norm, Vec3 &dir_in, float &pdf);

    /// @brief given dir_in, dir_out and normal, set pdf
    virtual void pdf(RNG *rng, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm, float &pdf);

    /// @brief if the sampler is specular
    virtual bool is_specular();
};
#endif