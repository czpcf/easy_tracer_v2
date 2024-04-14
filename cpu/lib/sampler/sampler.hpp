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
#include "../bxdf/bxdf.hpp"
#include "onb.hpp"


// this sampler class ONLY supports pure continuous pdf or pure disctrete pdf,
// cannot combine them together
class Sampler {
public:
    Sampler();
    virtual ~Sampler() = default;

    /// @brief given dir_in and normal, set dir_out and pdf
    /// if return false, failed to sample,
    /// if pdf < 0, descrete pdf
    virtual bool sample_out(const Surface &surface, RNG *rng, const Vec3 &dir_in, Vec3 &dir_out, float &pdf);

    /// @brief given dir_out and normal, set dir_in and pdf
    /// if return false, failed to sample
    /// if pdf < 0, descrete pdf
    virtual bool sample_in(const Surface &surface, RNG *rng, const Vec3 &dir_out, Vec3 &dir_in, float &pdf);

    /// @brief return sampled direction pdf
    virtual float pdf(const Surface &surface, const Vec3 &given, const Vec3 &sampled);

    /// @brief if the sampler is specular(have discrete pdf)
    virtual bool is_specular();
};
#endif