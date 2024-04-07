/*
ggx sampler
*/
#ifndef SAMPLER_GGX_H
#define SAMPLER_GGX_H

#include "sampler.hpp"

class SamplerGGX: public Sampler {
public:
    SamplerGGX();
    SamplerGGX(float _r, float _x, float _y);
    ~SamplerGGX() override = default;

    /// @brief given dir_in and normal, set dir_out and pdf
    /// if return false, failed to sample
    bool sample_out(const Surface &surface, RNG *rng, const Vec3 &dir_in, Vec3 &dir_out, float &pdf) override;

    /// @brief given dir_out and normal, set dir_in and pdf
    /// if return false, failed to sample
    bool sample_in(const Surface &surface, RNG *rng, const Vec3 &dir_out, Vec3 &dir_in, float &pdf) override;

    /// @brief set sampled direction pdf
    float pdf(const Surface &surface, const Vec3 &given, const Vec3 &sampled) override;
    
    /// @brief if the sampler is specular
    bool is_specular() override;

private:
    float change_pdf;
    float roughness;
    float alpha_x;
    float alpha_y;
};

#endif