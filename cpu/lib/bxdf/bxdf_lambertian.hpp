#ifndef BXDF_LAMBERTIAN_H
#define BXDF_LAMBERTIAN_H

#include "bxdf.hpp"

class BxdfLambertian: Bxdf {
public:
    BxdfLambertian();
    BxdfLambertian(float n);
    ~BxdfLambertian() override = default;

    /// @brief return phase function(in, out, normal)
    float phase(const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) override;

private:
    float alpha;
};

#endif