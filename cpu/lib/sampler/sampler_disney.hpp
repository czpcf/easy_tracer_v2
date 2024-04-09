/*
disney sampler
*/
#ifndef SAMPLER_DISNEY_H
#define SAMPLER_DISNEY_H

#include "sampler.hpp"

class SamplerDisney: public Sampler {
public:
    SamplerDisney();
    SamplerDisney(
        float _metallic,
        float _subsurface,
        float _specular,
        float _specular_tint,
        float _roughness,
        float _anisotropic,
        float _sheen,
        float _sheen_tint,
        Vec3 _clearcoat,
        float _clearcoat_gloss
    );
    ~SamplerDisney() override = default;

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
    float sigma_m; // metallic
    float sigma_ss; // subsurface
    float sigma_s; // specular
    float sigma_st; // specular_tint
    float sigma_r; // roughness
    float sigma_a; // anisotropic
    float sigma_sh; // sheen
    float sigma_sht; // sheen_tint
    Vec3 sigma_c; // clearcoat
    float sigma_cg; // clearcoat_gloss
    float pdf_d;
    float pdf_s;
    float pdf_c;
    float alpha_x;
    float alpha_y;
    float alpha;
    float alpha_c;
};

#endif