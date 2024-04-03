/*
ray_hit
intersection can be calculated easily;
normal, color, bxdf, texture should be in the object pool
*/
#ifndef RAY_HIT_H
#define RAY_HIT_H

#include "../tensor/vec2.hpp"
#include "../tensor/vec3.hpp"
#include "../ray/ray.hpp"

class RayHit {
public:
    RayHit();
    ~RayHit() = default;
    RayHit(int, float, const Vec2 &);

    /// @brief set ray-hit
    void set(int, float, const Vec2 &);

    /// @brief update ray-hit according to the distance
    void update(int, float, const Vec2 &);

    /// @brief return hit object id
    const int get_id() const;

    /// @brief return hit distance
    const float get_dis() const;

    /// @brief return hit object uv coordinate
    const Vec2 &get_local() const;

    /// @brief return p + t * d
    Vec3 get_inter(const Vec3 &p, const Vec3 &d);

    /// @brief return p + t * d
    Vec3 get_inter(const Ray &ray);

    /// @brief set hit id
    void set_id(int);

private:
    /// @brief ray hit id
    int id;

    /// @brief parameter t
    float dis;

    /// @brief ray hit local coordinate, not uv coordinate
    Vec2 local;
};
#endif
