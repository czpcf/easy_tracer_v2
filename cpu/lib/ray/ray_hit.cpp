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

int RayHit::get_id() {
    return id;
}

float RayHit::get_dis() {
    return dis;
}

const Vec2 &RayHit::get_local() const {
    return local;
}

Vec3 RayHit::get_inter(const Vec3 &p, const Vec3 &d) {
    return p + d * dis;
}
