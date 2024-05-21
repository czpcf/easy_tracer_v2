#include "curve.hpp"
#include <iostream>
#include <cmath>

BezierCurve::BezierCurve(const std::vector<Vec3> &points) : Curve(points) {
    if (points.size() < 4 || points.size() % 3 != 1) {
        fprintf(stderr, "Number of control points of BezierCurve must be 3n+1!\n");
        exit(0);
    }
}

const void BezierCurve::discretize(int resolution, std::vector<CurvePoint>& data) const {
    data.clear();
    constexpr int k = 3;
    int n = controls.size() - 1;
    std::vector<float>Cn;
    std::vector<float>Cn_1;
    for(int i = 0; i <= n; ++i) {
        Cn.push_back(0.0f);
    }
    Cn[0] = 1.0;
    for(int i = 1; i <= n; ++i) {
        for(int j = i; j >= 1; --j) {
            Cn[j] += Cn[j - 1];
        }
    }
    for(int i = 0; i <= n - 1; ++i) {
        Cn_1.push_back(0.0f);
    }
    Cn_1[0] = 1.0;
    for(int i = 1; i <= n - 1; ++i) {
        for(int j = i; j >= 1; --j) {
            Cn_1[j] += Cn_1[j - 1];
        }
    }
    std::vector<float>knot;
    for(int i = 0; i <= n; ++i) {
        knot.push_back(float(i) / n);
    }
    for(int pivot = 0; pivot < n; pivot += 3) {
        for(int d = 0; d < resolution + (pivot + 3 == n); ++d) {
            float t = (float(resolution - d) * knot[pivot] + float(d) * knot[pivot + 3] ) / resolution;
            if(t < 1e-6) {
                t = 1e-6;
            }
            if(t > 1.0f - 1e-6) {
                t = 1.0f - 1e-6;
            }
            Vec3 p(0.0f, 0.0f, 0.0f);
            Vec3 v(0.0f, 0.0f, 0.0f);
            for(int i = 0; i <= n; ++i) {
                p += controls[i] * (Cn[i] * powf(1.0f - t, n - i) * powf(t, i));
                float b = 0;
                if(i != 0) {
                    b += Cn_1[i - 1] * powf(1.0f - t, n - (i - 1)) * powf(t, i - 1);
                }
                if(i != n) {
                    b -= Cn_1[i] * powf(1.0f - t, n - i) * powf(t, i);
                }
                b *= n;
                v += controls[i] * b;
            }
            v.normalize();
            data.push_back((CurvePoint){p, v});
        }
    }
}


BsplineCurve::BsplineCurve(const std::vector<Vec3> &points) : Curve(points) {
    if (points.size() < 4) {
        fprintf(stderr, "Number of control points of BspineCurve must be more than 4!\n");
        exit(0);
    }
}

const void BsplineCurve::discretize(int resolution, std::vector<CurvePoint>& data) const {
    data.clear();
    constexpr int k = 3;
    int n = controls.size() - 1;
    std::vector<float>knot;
    for(int i = 0; i <= n + k + 1; ++i) {
        knot.push_back(float(i) / (n + k + 1));
    }
    float **a = new float*[n + k + 1];
    for(int i = 0; i <= n + k; ++i) {
        a[i] = new float[k + 1];
    }
    for(int i = 0; i <= n + k; ++i) {
        for(int j = 0; j <= k; ++j) {
            a[i][j] = 0.0f;
        }
    }
    auto get_b = [&](float t){
        for(int i = 0; i < n + k + 1; ++i) {
            if(knot[i] <= t && t < knot[i + 1]) {
                a[i][0] = 1.0f;
            } else {
                a[i][0] = 0.0f;
            }
        }
        for(int p = 1; p <= k; ++p) {
            for(int i = 0; i < n + k + 1 - p; ++i) {
                a[i][p] = 
                    (t - knot[i]) / (knot[i + p] - knot[i]) * a[i][p - 1] +
                    (knot[i + p + 1] - t) / (knot[i + p + 1] - knot[i + 1]) * a[i + 1][p - 1];
            }
        }
    };
    for(int pivot = k; pivot <= n; ++pivot) {
        for(int d = 0; d < resolution + (pivot == n); ++d) {
            float t = (float(resolution - d) * knot[pivot] + float(d) * knot[pivot + 1]) / resolution;
            if(t < 1e-6) {
                t = 1e-6;
            }
            if(t > 1.0f - 1e-6) {
                t = 1.0f - 1e-6;
            }
            Vec3 p(0.0f, 0.0f, 0.0f);
            Vec3 v(0.0f, 0.0f, 0.0f);
            get_b(t);
            for(int i = 0; i <= n; ++i) {
                p += controls[i] * a[i][k];
                v += controls[i] * 
                    ((a[i][k - 1] / (knot[i + k] - knot[i]) - a[i + 1][k - 1] / (knot[i + k + 1] - knot[i + 1])) * k);
            }
            v.normalize();
            data.push_back((CurvePoint){p, v});
        }
    }
    for(int i = 0; i <= n + k; ++i) {
        delete[] a[i];
    }
    delete[] a;
}