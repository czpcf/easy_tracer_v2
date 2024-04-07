/*
bvh accel
*/
#ifndef ACCEL_BVH_H
#define ACCEL_BVH_H

#include "accel.hpp"
#include "../random/random_mt19937.hpp"

class AccelBVH: public Accel {
public:
    AccelBVH();
    ~AccelBVH() override = default;

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

    /// @brief check if the ray hits the shape given the id,
    /// distance should be squared
    bool if_inter_id(const Ray&, int) override;
    
    /// @brief build accelerator
    void build() override;

private:
    /// @brief build tree, using surface area heuristic
    void build_tree(uint l, uint r, uint &tot, RNGMT19937 &rng, uint dep);

    /// @brief actual id for pointers
    std::vector<int>id;

    std::vector<uint> rs;
    std::vector<uint> nxt;
    std::vector<Geometry*> shapes_this;
    std::vector<std::pair<Box, uint> > aux_boxes;
    std::vector<Box> aux_boxes_buf;
    uint height;
    uint have;
};
#endif