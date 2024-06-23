/*
triangle
*/
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "geometry.hpp"
#include "../tensor/vec3.hpp"
#include "../tensor/mat3.hpp"

class Triangle: public Geometry {
public:
    Triangle();
    /// @brief counter-clockwise: p1 -> p2 -> p3
    Triangle(Vec3, Vec3, Vec3);
    
    /// @brief intersection with the line from point p in direction d
    /// if intersected, return true and set ray-hit
    /// otherwise return false and do nothing
    bool inter(const Ray &, RayHit &) override;

    /// @brief intersection with the line from point p in direction d
    /// if intersected, return true and set ray-hit
    /// otherwise return false and do nothing
    bool inter_update(const Ray &, RayHit &) override;
    
    /// @brief check if the ray hits the shape given the distance,
    /// distance should be squared
    bool if_inter_dis(const Ray &, float) override;

    /// @brief convert intersecion into local
    Vec2 inter_to_local(const Vec3 &) override;

    /// @brief apply the transform to the geometry permanently
    void trans(Mat3& T) override;

    /// @brief debug using fprintf
    void debug() override;

    /// @brief return bounding box
    Box bound() override;

    /// @brief return area
    float area() override;

    /// @brief return norm
    const Vec3 get_norm() const;

    /// @brief return p1
    const Vec3 get_p1() const;

    /// @brief return p2
    const Vec3 get_p2() const;

    /// @brief return p3
    const Vec3 get_p3() const;

    /// @brief return interpolated norm according to local
    static const Vec3 interpolate_norm(const Vec3 &n1, const Vec3 &n2, const Vec3 &n3, const Vec2 &local);

    /// @brief return interpolated uv according to local
    static const UV interpolate_uv(const UV &u1, const UV &u2, const UV &u3, const Vec2 &local);

private:
    /// @brief fast pre-calculation
    unsigned short type;
    /// @brief area
    float _area;
    /// @brief 3 vertices of the triangle
    Vec3 _p1, _p2, _p3;
    /// @brief normal of the triangle
    Vec3 norm;
};
#endif