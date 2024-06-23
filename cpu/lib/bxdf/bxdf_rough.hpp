#ifndef BXDF_ROUGH_H
#define BXDF_ROUGH_H

#include "bxdf.hpp"

class BxdfRough: public Bxdf {
public:
    BxdfRough();
    BxdfRough(float alpha);
    ~BxdfRough() override = default;

    /// @brief return phase function(in, out, normal)
    /// dir_in is surface -> light
    Vec3 phase(const Surface &surface, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) override;

private:
    float alpha;
};

#endif