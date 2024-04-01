/*
perfect diffuse sampler
*/
#ifndef SAMPLER_LAMBERTIAN_H
#define SAMPLER_LAMBERTIAN_H

#include "sampler.hpp"

class SamplerLambertian: Sampler {
public:
    SamplerLambertian();
    ~SamplerLambertian() override = default;

    /// @brief given dir_in and normal, set dir_out and pdf
    void sample_out(RNG *rng, const Vec3 &dir_in, const Vec3 &norm, Vec3 &dir_out, float &pdf) override;

    /// @brief given dir_out and normal, set dir_in and pdf
    void sample_in(RNG *rng, const Vec3 &dir_out, const Vec3 &norm, Vec3 &dir_in, float &pdf) override;

    /// @brief given dir_in, dir_out and normal, set pdf
    void pdf(RNG *rng, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm, float &pdf) override;
    
    /// @brief if the sampler is specular
    bool is_specular() override;
};

#endif