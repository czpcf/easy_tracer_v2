/*
naive accel
just using array
*/
#ifndef ACCEL_NAIVE
#define ACCEL_NAIVE

#include "accel.hpp"

class AccelNaive: public Accel {
public:
    AccelNaive();
    ~AccelNaive() override = default;

    /// @brief set size of elements
    void set_size(int n) override;

    /// @brief add single resource
    void add(Geometry *) override;

    /// @brief add resource group
    void add(ResourceGroup *) override;

    /// @brief intersection with the line from point p in direction d
    /// if intersected, return true and set ray-hit
    /// otherwise return false and do nothing
    bool inter(const Ray&, RayHit &) override;

    /// @brief check if the ray hits the shape given the distance,
    /// distance should be squared
    bool if_inter_dis(const Ray&, float) override;
    
    /// @brief build accelerator
    void build() override;
};
#endif