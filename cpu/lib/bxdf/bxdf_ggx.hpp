/*
GGX
using Torrance-Sparrow model
NDF: GGX
Masking term: Smith
Fresnel: Schlick
*/


#ifndef BXDF_GGX_H
#define BXDF_GGX_H

#include "bxdf.hpp"

class BxdfGGX: public Bxdf {
public:
    BxdfGGX();
    BxdfGGX(float rough, float ax, float ay);
    ~BxdfGGX() override = default;

    /// @brief return phase function(in, out, normal)
    Vec3 phase(const Surface &surface, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) override;

    /// @brief static phase
    static Vec3 phase(
        float _roughness, float _alpha_x, float _alpha_y,
        const Surface &surface, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm
    );

private:
    float roughness; // actually NOT physically based !
    float alpha_x;
    float alpha_y;
};

#endif