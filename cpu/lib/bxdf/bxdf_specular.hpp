#ifndef BXDF_SPECULAR_H
#define BXDF_SPECULAR_H

#include "bxdf.hpp"

class BxdfSpecular: public Bxdf {
public:
    BxdfSpecular();
    ~BxdfSpecular() override = default;

    /// @brief return phase function(in, out, normal)
    Vec3 phase(const Surface &surface, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) override;

private:
};

#endif