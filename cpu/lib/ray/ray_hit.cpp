#include "ray_hit.hpp"

RayHit::RayHit() {
    dis = 1e38;
}

RayHit::RayHit(int x, float t, const Vec2 &l): id(x), dis(t), local(l) {
}

void RayHit::set(int x, float t, const Vec2 &l) {
    id = x;
    dis = t;
    local = l;
}

void RayHit::update(int x, float t, const Vec2 &l) {
    if(t < dis) {
        set(x, t, l);
    }
}

const int RayHit::get_id() const {
    return id;
}

const float RayHit::get_dis() const {
    return dis;
}

const Vec2 &RayHit::get_local() const {
    return local;
}

Vec3 RayHit::get_inter(const Vec3 &p, const Vec3 &d) {
    return p + d * dis;
}

Vec3 RayHit::get_inter(const Ray &ray) {
    return ray.get_origin() + ray.get_direction() * dis;
}

void RayHit::set_id(int x) {
    id = x;
}