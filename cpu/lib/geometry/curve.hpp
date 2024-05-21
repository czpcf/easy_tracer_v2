#ifndef CURVE_HPP
#define CURVE_HPP

#include "geometry.hpp"
#include <vector>

// easy struct for curve points
struct CurvePoint {
    Vec3 V; // Vertex
    Vec3 T; // Tangent
};

// does not support 2d curves,
// so does not inherit from geometry
class Curve {
protected:
    std::vector<Vec3> controls;
public:
    explicit Curve(std::vector<Vec3> points) : controls(std::move(points)) {}

    const virtual void discretize(int resolution, std::vector<CurvePoint>& data) const = 0;
};


// Bezier
class BezierCurve : public Curve {
public:
    explicit BezierCurve(const std::vector<Vec3> &points);

    const void discretize(int resolution, std::vector<CurvePoint>& data) const override;
};

// Bspline
// k=3, and the intervel is uniformly segmented
class BsplineCurve : public Curve {
public:
    BsplineCurve(const std::vector<Vec3> &points);

    const void discretize(int resolution, std::vector<CurvePoint>& data) const override;
};
#endif
