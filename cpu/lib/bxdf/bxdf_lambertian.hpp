#ifndef BXDF_LAMBERTIAN_H
#define BXDF_LAMBERTIAN_H

#include "bxdf.hpp"

class BxdfLambertian: public Bxdf {
public:
    BxdfLambertian();
    ~BxdfLambertian() override = default;

    /// @brief return phase function(in, out, normal)
    Vec3 phase(const Surface &surface, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) override;

private:
};

#endif