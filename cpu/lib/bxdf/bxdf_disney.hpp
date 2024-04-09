/*
disney bxdf
*/


#ifndef BXDF_DISNEY_H
#define BXDF_DISNEY_H

#include "bxdf.hpp"

class BxdfDisney: public Bxdf {
public:
    BxdfDisney();
    BxdfDisney(
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
    ~BxdfDisney() override = default;

    /// @brief return phase function(in, out, normal)
    Vec3 phase(const Surface &surface, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) override;

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

    float alpha_x;
    float alpha_y;
    float alpha;
    float alpha_c;
};

#endif