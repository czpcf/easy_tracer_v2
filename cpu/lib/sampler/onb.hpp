/*
orthogonal normal base
*/
#ifndef ONB_H
#define ONB_H

#include "../const.hpp"
#include "../tensor/vec3.hpp"
#include "../tensor/vec2.hpp"
#include "../random/random.hpp"
#include <cmath>

class Onb {
public:
    Onb();
    ~Onb();
    /// @brief make Onb with the outter-pointing normal
    Onb(Vec3 normal_g);

    /// @brief get the sample in the world
    Vec3 to_world();

    /// @brief set local to (sqrt(ux)*cos(2pi*uy), sqrt(ux)*sin(2pi*uy), sqrt(1-ux)), using rng
    void cosine_sample_hemisphere(RNG *rng);

    /// @brief set local to (sqrt(ux)*cos(2pi*uy), sqrt(ux)*sin(2pi*uy), sqrt(1-ux))
    void cosine_sample_hemisphere(Vec2 uv);

    /// @brief return pdf: abs(norm.Dot(dir_out)) / PI;
    static float pdf_cosine_sample_hemisphere(const Vec3 &dir_out, const Vec3 &norm);

    /// @brief return last sampling pdf
    float pdf();

private:
    Vec3 _normal;
    Vec3 _binormal;
    Vec3 _tangent;
    Vec3 _local;
    float _pdf;
};
#endif
