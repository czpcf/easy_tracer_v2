#include <iostream>
#include <assert.h>
#include <chrono>
#include <random>
#include "../lib/easy_tracer_v2.hpp"
using namespace std;
mt19937 RD(123);
float R() {
    return (float)((RD() % 2147483648) - 1073741824) / 1073741824;
}
void CheckInter() {
    auto start = chrono::high_resolution_clock::now();
    Sphere O(Vec3(0, 0, 1), Vec3(1, 0, 0), Vec3(0, 0, 0), 2);
    Box b = O.bound();
    Vec3 p(0, 0, 0);
    for(int i = 0; i < 10000000; ++i) {
        Vec3 d(1, R() / 10, R() / 10);
        auto res = b.inter(p, d);
        Vec3 inter = p + d * res.second;
        assert(abs(inter.x - 2) <= 1e-6);
    }
    for(int i = 0; i < 10000000; ++i) {
        Vec3 d(-1, R() / 10, R() / 10);
        auto res = b.inter(p, d);
        Vec3 inter = p + d * res.second;
        assert(abs(inter.x + 2) <= 1e-6);
    }
    for(int i = 0; i < 10000000; ++i) {
        Vec3 d(R() / 10, 1, R() / 10);
        auto res = b.inter(p, d);
        Vec3 inter = p + d * res.second;
        assert(abs(inter.y - 2) <= 1e-6);
    }
    for(int i = 0; i < 10000000; ++i) {
        Vec3 d(R() / 10, -1, R() / 10);
        auto res = b.inter(p, d);
        Vec3 inter = p + d * res.second;
        assert(abs(inter.y + 2) <= 1e-6);
    }
    for(int i = 0; i < 10000000; ++i) {
        Vec3 d(R() / 10, R() / 10, 1);
        auto res = b.inter(p, d);
        Vec3 inter = p + d * res.second;
        assert(abs(inter.z - 2) <= 1e-6);
    }
    for(int i = 0; i < 10000000; ++i) {
        Vec3 d(R() / 10, R() / 10, -1);
        auto res = b.inter(p, d);
        Vec3 inter = p + d * res.second;
        assert(abs(inter.z + 2) <= 1e-6);
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "inter time used: " << (double)(duration.count()) / 1000 << " ms" << endl;
}
void CheckInterIf() {
    auto start = chrono::high_resolution_clock::now();
    Sphere O(Vec3(0, 0, 1), Vec3(1, 0, 0), Vec3(0, 0, 0), 2);
    Box b = O.bound();
    Vec3 p(0, 0, 0);
    for(int i = 0; i < 10000000; ++i) {
        Vec3 d(1, R() / 10, R() / 10);
        assert(b.inter_if(p, d));
    }
    for(int i = 0; i < 10000000; ++i) {
        Vec3 d(-1, R() / 10, R() / 10);
        assert(b.inter_if(p, d));
    }
    for(int i = 0; i < 10000000; ++i) {
        Vec3 d(R() / 10, 1, R() / 10);
        assert(b.inter_if(p, d));
    }
    for(int i = 0; i < 10000000; ++i) {
        Vec3 d(R() / 10, -1, R() / 10);
        assert(b.inter_if(p, d));
    }
    for(int i = 0; i < 10000000; ++i) {
        Vec3 d(R() / 10, R() / 10, 1);
        assert(b.inter_if(p, d));
    }
    for(int i = 0; i < 10000000; ++i) {
        Vec3 d(R() / 10, R() / 10, -1);
        assert(b.inter_if(p, d));
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "inter_if time used: " << (double)(duration.count()) / 1000 << " ms" << endl;
}
void CheckInterNormed() {
    auto start = chrono::high_resolution_clock::now();
    Sphere O(Vec3(0, 0, 1), Vec3(1, 0, 0), Vec3(0, 0, 0), 2);
    Box b = O.bound();
    Vec3 p(0, 0, 0);
    for(int i = 0; i < 10000000; ++i) {
        float x = R() / 10;
        float y = R() / 10;
        Vec3 d(2, x, y);
        d.normalize();
        float ndis = b.inter_normed(p, d);
        assert(abs(ndis * ndis - (4.0f + x * x + y * y)) <= 1e-6);
    }
    for(int i = 0; i < 10000000; ++i) {
        float x = R() / 10;
        float y = R() / 10;
        Vec3 d(-2, x, y);
        d.normalize();
        float ndis = b.inter_normed(p, d);
        assert(abs(ndis * ndis - (4.0f + x * x + y * y)) <= 1e-6);
    }
    for(int i = 0; i < 10000000; ++i) {
        float x = R() / 10;
        float y = R() / 10;
        Vec3 d(x, 2, y);
        d.normalize();
        float ndis = b.inter_normed(p, d);
        assert(abs(ndis * ndis - (4.0f + x * x + y * y)) <= 1e-6);
    }
    for(int i = 0; i < 10000000; ++i) {
        float x = R() / 10;
        float y = R() / 10;
        Vec3 d(x, -2, y);
        d.normalize();
        float ndis = b.inter_normed(p, d);
        assert(abs(ndis * ndis - (4.0f + x * x + y * y)) <= 1e-6);
    }
    for(int i = 0; i < 10000000; ++i) {
        float x = R() / 10;
        float y = R() / 10;
        Vec3 d(x, y, 2);
        d.normalize();
        float ndis = b.inter_normed(p, d);
        assert(abs(ndis * ndis - (4.0f + x * x + y * y)) <= 1e-6);
    }
    for(int i = 0; i < 10000000; ++i) {
        float x = R() / 10;
        float y = R() / 10;
        Vec3 d(x, y, -2);
        d.normalize();
        float ndis = b.inter_normed(p, d);
        assert(abs(ndis * ndis - (4.0f + x * x + y * y)) <= 1e-6);
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "inter_normed time used(slow due to too much normalization): " << (double)(duration.count()) / 1000 << " ms" <<endl;
}
void CheckInterSquared() {
    auto start = chrono::high_resolution_clock::now();
    Sphere O(Vec3(0, 0, 1), Vec3(1, 0, 0), Vec3(0, 0, 0), 2);
    Box b = O.bound();
    Vec3 p(0, 0, 0);
    for(int i = 0; i < 10000000; ++i) {
        float x = R() / 10;
        float y = R() / 10;
        Vec3 d(2, x, y);
        float ndis = b.inter_squared(p, d);
        assert(abs(ndis - (4.0f + x * x + y * y)) <= 1e-6);
    }
    for(int i = 0; i < 10000000; ++i) {
        float x = R() / 10;
        float y = R() / 10;
        Vec3 d(-2, x, y);
        float ndis = b.inter_squared(p, d);
        assert(abs(ndis - (4.0f + x * x + y * y)) <= 1e-6);
    }
    for(int i = 0; i < 10000000; ++i) {
        float x = R() / 10;
        float y = R() / 10;
        Vec3 d(x, 2, y);
        float ndis = b.inter_squared(p, d);
        assert(abs(ndis - (4.0f + x * x + y * y)) <= 1e-6);
    }
    for(int i = 0; i < 10000000; ++i) {
        float x = R() / 10;
        float y = R() / 10;
        Vec3 d(x, -2, y);
        float ndis = b.inter_squared(p, d);
        assert(abs(ndis - (4.0f + x * x + y * y)) <= 1e-6);
    }
    for(int i = 0; i < 10000000; ++i) {
        float x = R() / 10;
        float y = R() / 10;
        Vec3 d(x, y, 2);
        float ndis = b.inter_squared(p, d);
        assert(abs(ndis - (4.0f + x * x + y * y)) <= 1e-6);
    }
    for(int i = 0; i < 10000000; ++i) {
        float x = R() / 10;
        float y = R() / 10;
        Vec3 d(x, y, -2);
        float ndis = b.inter_squared(p, d);
        assert(abs(ndis - (4.0f + x * x + y * y)) <= 1e-6);
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "inter_squared time used: " << (double)(duration.count()) / 1000 << " ms" << endl;
}
void CheckInterSquaredNormed() {
    auto start = chrono::high_resolution_clock::now();
    Sphere O(Vec3(0, 0, 1), Vec3(1, 0, 0), Vec3(0, 0, 0), 2);
    Box b = O.bound();
    Vec3 p(0, 0, 0);
    for(int i = 0; i < 10000000; ++i) {
        float x = R() / 10;
        float y = R() / 10;
        Vec3 d(2, x, y);
        float ndis = b.inter_squared_normed(p, d);
        assert(abs(ndis * (4.0f + x * x + y * y) - (4.0f + x * x + y * y)) <= 1e-6);
    }
    for(int i = 0; i < 10000000; ++i) {
        float x = R() / 10;
        float y = R() / 10;
        Vec3 d(-2, x, y);
        float ndis = b.inter_squared_normed(p, d);
        assert(abs(ndis * (4.0f + x * x + y * y) - (4.0f + x * x + y * y)) <= 1e-6);
    }
    for(int i = 0; i < 10000000; ++i) {
        float x = R() / 10;
        float y = R() / 10;
        Vec3 d(x, 2, y);
        float ndis = b.inter_squared_normed(p, d);
        assert(abs(ndis * (4.0f + x * x + y * y) - (4.0f + x * x + y * y)) <= 1e-6);
    }
    for(int i = 0; i < 10000000; ++i) {
        float x = R() / 10;
        float y = R() / 10;
        Vec3 d(x, -2, y);
        float ndis = b.inter_squared_normed(p, d);
        assert(abs(ndis * (4.0f + x * x + y * y) - (4.0f + x * x + y * y)) <= 1e-6);
    }
    for(int i = 0; i < 10000000; ++i) {
        float x = R() / 10;
        float y = R() / 10;
        Vec3 d(x, y, 2);
        float ndis = b.inter_squared_normed(p, d);
        assert(abs(ndis * (4.0f + x * x + y * y) - (4.0f + x * x + y * y)) <= 1e-6);
    }
    for(int i = 0; i < 10000000; ++i) {
        float x = R() / 10;
        float y = R() / 10;
        Vec3 d(x, y, -2);
        float ndis = b.inter_squared_normed(p, d);
        assert(abs(ndis * (4.0f + x * x + y * y) - (4.0f + x * x + y * y)) <= 1e-6);
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "inter_squared_normed time used: " << (double)(duration.count()) / 1000 << " ms" << endl;
}
int main() {
    CheckInter();
    CheckInterIf();
    CheckInterNormed();
    CheckInterSquared();
    CheckInterSquaredNormed();
    return 0;
}
