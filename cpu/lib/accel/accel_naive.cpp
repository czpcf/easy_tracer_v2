#include "accel_naive.hpp"


AccelNaive::AccelNaive() {
}

void AccelNaive::set_size(int n) {
    boxes.reserve(n);
    shapes.reserve(n);
}

void AccelNaive::add(Geometry* shape) {
    boxes.push_back(shape->bound());
    shapes.push_back(shape);
}

void AccelNaive::add(ResourceGroup *group) {
    int n = group->n_objects();
    for(int i = 0; i < n; ++i) {
        boxes.push_back(group->get_shape(i)->bound());
        shapes.push_back(group->get_shape(i));
    }
}

bool AccelNaive::inter(const Ray &ray, RayHit &hit) {
    hit.set(-1, 1e38, Vec2());
    int n = boxes.size();
    bool yes = false;
    for(int i = 0; i < n; ++i) {
        if(shapes[i]->inter_update(ray, hit)) {
            hit.set_id(i);
            yes = true;
        }
    }
    return yes;
}

bool AccelNaive::if_inter_dis(const Ray &ray, float dis) {
    int n = boxes.size();
    for(int i = 0; i < n; ++i) {
        if(shapes[i]->if_inter_dis(ray, dis)) {
            return true;
        }
    }
    return false;
}


bool AccelNaive::if_inter_id(const Ray &ray, int id) {
    RayHit hit;
    hit.set(-1, 1e38, Vec2());
    int n = boxes.size();
    bool yes = false;
    for(int i = 0; i < n; ++i) {
        if(shapes[i]->inter_update(ray, hit)) {
            hit.set_id(i);
            yes = true;
        }
    }
    return (yes & (hit.get_id() == id));
}

void AccelNaive::build() {
    // do nothing
}