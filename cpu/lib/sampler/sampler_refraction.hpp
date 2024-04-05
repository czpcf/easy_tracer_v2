/*
perfect refraction sampler
*/
#ifndef SAMPLER_REFRACTION_H
#define SAMPLER_REFRACTION_H

#include "sampler.hpp"

class SamplerRefraction: public Sampler {
public:
    SamplerRefraction();
    SamplerRefraction(float eta);
    ~SamplerRefraction() override = default;

    /// @brief given dir_in and normal, set dir_out and pdf
    void sample_out(RNG *rng, const Vec3 &dir_in, const Vec3 &norm, Vec3 &dir_out, float &pdf) override;

    /// @brief given dir_out and normal, set dir_in and pdf
    void sample_in(RNG *rng, const Vec3 &dir_out, const Vec3 &norm, Vec3 &dir_in, float &pdf) override;

    /// @brief set sampled direction pdf
    float pdf(const Vec3 &given, const Vec3 &sampled, const Vec3 &norm) override;

    /// @brief if the sampler is specular
    bool is_specular() override;

private:

    /// @brief refraction ration
    float n;
};

#endif