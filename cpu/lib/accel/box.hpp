/*
bounding box
*/
#include <utility>
#include "../tensor/vec3.hpp"
#ifndef BOX_H
#define BOX_H
class Box {
public:
    Box();

    /// @brief initialization with two points
    Box(const Vec3 &, const Vec3 &);
    ~Box() = default;
    Vec3 get_lower();
    Vec3 get_upper();
    Vec3 get_mid();

    /// @brief add one more point, then change the bound
    void add(const Vec3 &);

    /// @brief add one more box, then change the bound
    void add(const Box &);

    /// @brief if the point is inside the box
    bool inside(const Vec3 &);

    /// @brief return whether intersected or not, for improved speed
    bool inter_if(const Vec3 &p, const Vec3 &d);

    /// @brief assuming d is normalized
    float inter_normed(const Vec3 &p, const Vec3 &d);

    /// @brief return length^2
    float inter_squared(const Vec3 &p, const Vec3 &d);

    /// @brief return length^2 and assume d is normalized
    float inter_squared_normed(const Vec3 &p, const Vec3 &d);

    /// @brief return length^2 and assume d is normalized, p is outside the box
    float inter_squared_normed_outside(const Vec3 &p, const Vec3 &d);

    /// @brief return length and assume d is normalized, p is outside the box
    float inter_normed_outside(const Vec3 &p, const Vec3 &d);
    
    /// @brief check if the length_x is < dis and assume d is normalized, p is outside the box
    bool check_normed_outsideX(const Vec3 &p, const Vec3 &d, float dis);

    /// @brief if not intersected, return <false, inf>
    std::pair<bool, float> inter(const Vec3 &p, const Vec3 &d);

    /// @brief return area
    float area();

    /// @brief return area / 2
    float half_area();

    const Vec3 &lower() const;
    const Vec3 &upper() const;
    const Vec3 &mid() const;

private:
    /// point with smaller values
    Vec3 _lower;
    
    /// point with bigger values
    Vec3 _upper;

    /// point in the middle
    Vec3 _mid;
};
#endif
