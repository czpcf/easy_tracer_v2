/*
bxdf
*/
#ifndef BXDF_H
#define BXDF_H

#include "../tensor/vec2.hpp"
#include "../tensor/vec3.hpp"
#include "../tensor/mat3.hpp"

class Bxdf {
public:
    Bxdf();
    virtual ~Bxdf() = default;

    /// @brief return phase function(in, out, normal)
    virtual float phase(const Vec3 &dir_in, const Vec3 &dir_out, const Vec3 &norm);
};

#endif