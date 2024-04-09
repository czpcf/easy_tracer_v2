/*
GTR1
using Torrance-Sparrow model
NDF: GTR1
Masking term: Smith
Fresnel: Schlick
*/


#ifndef BXDF_GTR1_H
#define BXDF_GTR1_H

#include "bxdf.hpp"

class BxdfGTR1: public Bxdf {
public:
    BxdfGTR1();
    BxdfGTR1(float _alpha);
    ~BxdfGTR1() override = default;

    /// @brief return phase function(in, out, normal)
    Vec3 phase(const Surface &surface, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) override;

    /// @brief return phase function(in, out, normal)
    static Vec3 phase(
        float _alpha,
        const Surface &surface, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm
    );

private:
    float alpha;
    float alpha2;
};

#endif