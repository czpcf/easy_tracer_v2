#ifndef BXDF_PHONG_H
#define BXDF_PHONG_H

#include "bxdf.hpp"

class BxdfPhong: public Bxdf {
public:
    BxdfPhong();
    BxdfPhong(const Vec3 &_diffuse, const Vec3 &_specular, float _shininess);
    ~BxdfPhong() override = default;

    /// @brief return phase function(in, out, normal)
    /// dir_in is surface -> light
    Vec3 phase(const Surface &surface, const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm) override;

private:
    Vec3 diffuse;
    Vec3 specular;
    float shininess;
};

#endif