#include "accel_bvh.hpp"

AccelBVH::AccelBVH() {
}

void AccelBVH::set_size(int n) {
    boxes.reserve(n * 2);
    rs.reserve(n * 2);
    nxt.reserve(n * 2);
    shapes_this.reserve(n * 2);
    shapes.reserve(n);
    id.reserve(n);
    aux_boxes.reserve(n);
    aux_boxes_buf.reserve(n);
}

void AccelBVH::add(Geometry* shape) {
    id.push_back(boxes.size());
    boxes.push_back(shape->bound());
    shapes.push_back(shape);
}

void AccelBVH::add(ResourceGroup *group) {
    int n = group->n_objects();
    for(int i = 0; i < n; ++i) {
        id.push_back(boxes.size());
        boxes.push_back(group->get_shape(i)->bound());
        shapes.push_back(group->get_shape(i));
    }
}

bool AccelBVH::inter(const Ray &ray, RayHit &hit) {
    hit.set(-1, 1e38, Vec2());
    bool yes = false;
    for(uint num = 0; num < have;) {
        uint g = rs[num];
        if(g & 1) {
            if(shapes_this[num]->inter_update(ray, hit)) {
                hit.set_id(rs[num] >> 1);
                yes = true;
            }
            ++num;
        } else if(!boxes[num].inside(ray.get_origin())) {
            if(boxes[num].inter_normed_outside(ray.get_origin(), ray.get_direction()) >= hit.get_dis()) {
                num = nxt[num];
            } else {
                ++num;
            }
        } else {
            ++num;
        }
    }
    return yes;
}

bool AccelBVH::if_inter_dis(const Ray &ray, float dis) {
    for(uint num = 0; num < have;) {
        uint g = rs[num];
        if(g & 1) {
            if(shapes_this[num]->if_inter_dis(ray, dis) == true) {
                return true;
            }
            ++num;
        } else if(!boxes[num].inside(ray.get_origin())) {
            if(boxes[num].inter_normed_outside(ray.get_origin(), ray.get_direction()) >= dis) {
                num = nxt[num];
            } else {
                ++num;
            }
        } else {
            ++num;
        }
    }
    return false;
}


bool AccelBVH::if_inter_id(const Ray &ray, int id) {
    RayHit hit;
    hit.set(-1, 1e38, Vec2());
    bool yes = false;
    for(uint num = 0; num < have;) {
        uint g = rs[num];
        if(g & 1) {
            if(shapes_this[num]->inter_update(ray, hit)) {
                hit.set_id(rs[num] >> 1);
                yes = true;
            }
            ++num;
        } else if(!boxes[num].inside(ray.get_origin())) {
            if(boxes[num].inter_normed_outside(ray.get_origin(), ray.get_direction()) >= hit.get_dis()) {
                num = nxt[num];
            } else {
                ++num;
            }
        } else {
            ++num;
        }
    }
    return (yes & (hit.get_id() == id));
}

void AccelBVH::build_tree(uint l, uint r, uint &tot, RNGMT19937 &rng, uint dep) {
    height = std::max(height, dep);
    if(l + 1 >= r) {
        rs[tot] = 1;
        shapes_this[tot] = shapes[0];
        ++tot;
        return;
    }
    float cost_x = 1e30, cost_y = 1e30, cost_z = 1e30;
    uint pos_x, pos_y, pos_z;
    Box box = aux_boxes[l].first;
    for(uint i = l + 1; i < r; ++i) {
        box.add(aux_boxes[i].first);
    }
    float area = box.half_area();
    auto sort_box = [&](float &cost_n, uint &pos_n){
        aux_boxes_buf[r - 1] = aux_boxes[r - 1].first;
        for(int i = int(r) - 2; i >= int(l); --i) {
            aux_boxes_buf[i] = aux_boxes_buf[i + 1];
            aux_boxes_buf[i].add(aux_boxes[i].first);
        }
        Box box_l = aux_boxes[l].first;
        float mid = float((l + r)>>1);
        for(int i = int(l) + 1; i < int(r); ++i) {
            float area_a = box_l.half_area();
            float area_b = aux_boxes_buf[i].half_area();
            float cost = (area_a + area_b) / area + (area_a / area_b + area_b / area_a);
            float bias = std::abs(float(i - mid) / (r - l));
            cost += bias * bias * 0.3f; // to control max depth
            if(cost < cost_n) {
                cost_n = cost;
                pos_n = i;
            }
            box_l.add(aux_boxes[i].first);
        }
    };
    // x-axis
    std::sort(aux_boxes.begin() + l, aux_boxes.begin() + r, [](std::pair<Box, uint> a, std::pair<Box, uint> b){
        return a.first.mid().x < b.first.mid().x;
    });
    sort_box(cost_x, pos_x);
    // y-axis
    std::sort(aux_boxes.begin() + l, aux_boxes.begin() + r, [](std::pair<Box, uint> a, std::pair<Box, uint> b){
        return a.first.mid().y < b.first.mid().y;
    });
    sort_box(cost_y, pos_y);
    // z-axis
    std::sort(aux_boxes.begin() + l, aux_boxes.begin() + r, [](std::pair<Box, uint> a, std::pair<Box, uint> b){
        return a.first.mid().z < b.first.mid().z;
    });
    sort_box(cost_z, pos_z);

    // recursion
    uint pos = (l + r)>>1;
    if(cost_x < cost_y && cost_x < cost_z) {
        std::sort(aux_boxes.begin() + l, aux_boxes.begin() + r, [](std::pair<Box, uint> a, std::pair<Box, uint> b){
            return a.first.mid().x < b.first.mid().x;
        });
        pos = pos_x;
    } else if(cost_y < cost_x && cost_y < cost_z) {
        std::sort(aux_boxes.begin() + l, aux_boxes.begin() + r, [](std::pair<Box, uint> a, std::pair<Box, uint> b){
            return a.first.mid().y < b.first.mid().y;
        });
        pos = pos_y;
    } else {
        std::sort(aux_boxes.begin() + l, aux_boxes.begin() + r, [](std::pair<Box, uint> a, std::pair<Box, uint> b){
            return a.first.mid().z < b.first.mid().z;
        });
        pos = pos_z;
    }
    uint num = tot;
    boxes[num] = box;
    ++tot;
    if(rng.rand_uint() & 1) {
        if(l == pos - 1) {
            int id = aux_boxes[pos - 1].second;
            rs[tot] = (id<<1) | 1;
            shapes_this[tot] = shapes[id];
            nxt[tot] = tot + 1;
            boxes[tot] = shapes[id]->bound();
            ++tot;
        } else {
            build_tree(l, pos, tot, rng, dep + 1);
        }
        if(pos + 1 == r) {
            int id = aux_boxes[pos].second;
            rs[num] = tot<<1;
            rs[tot] = (id<<1) | 1;
            shapes_this[tot] = shapes[id];
            nxt[tot] = tot + 1;
            boxes[tot] = shapes[id]->bound();
            ++tot;
        } else {
            rs[num] = tot<<1;
            build_tree(pos, r, tot, rng, dep + 1);
        }
    } else {
        if(pos + 1 == r) {
            int id = aux_boxes[pos].second;
            rs[tot] = (id<<1) | 1;
            shapes_this[tot] = shapes[id];
            nxt[tot] = tot + 1;
            boxes[tot] = shapes[id]->bound();
            ++tot;
        } else {
            build_tree(pos, r, tot, rng, dep + 1);
        }
        if(l == pos - 1) {
            int id = aux_boxes[pos - 1].second;
            rs[num] = tot<<1;
            rs[tot] = (id<<1) | 1;
            shapes_this[tot] = shapes[id];
            nxt[tot] = tot + 1;
            boxes[tot] = shapes[id]->bound();
            ++tot;
        } else {
            rs[num] = tot<<1;
            build_tree(l, pos, tot, rng, dep + 1);
        }
    }
    boxes[num] = boxes[num + 1];
    boxes[num].add(boxes[rs[num]>>1]);
    nxt[num] = std::max(nxt[num + 1], nxt[rs[num]>>1]);
}

void AccelBVH::build() {
    uint n = shapes.size();
    height = 0;
    while(rs.size() < 2 * n) {
        rs.push_back(0);
    }
    while(nxt.size() < 2 * n) {
        nxt.push_back(0);
    }
    while(boxes.size() < 2 * n) {
        boxes.push_back(Box());
    }
    for(uint i = 0; i < 2 * n; ++i) {
        rs[i] = nxt[i] = 0;
    }
    for(uint i = 0; i < n; ++i) {
        aux_boxes.push_back(std::make_pair(shapes[i] -> bound(), i));
        aux_boxes_buf.push_back(shapes[i] -> bound());
    }
    for(uint i = 0; i < n * 2; ++i) {
        shapes_this.push_back(nullptr);
    }
    uint tot = 0;
    RNGMT19937 rng(123);
    int l = 0; // for boxes with infinite areas
    int r = n; // for boxes with 0 area
    for(int i = 0; i < int(n); ++i) {
        if(aux_boxes[i].first.area() > 1e15) {
            int id = aux_boxes[i].second;
            rs[tot] = (id<<1) | 1;
            shapes_this[tot] = shapes[id];
            nxt[tot] = tot + 1;
            boxes[tot] = shapes[id]->bound();
            ++tot;
            swap(aux_boxes[i], aux_boxes[l]);
            ++l;
        }
    }
    for(int i = int(n - 1); i >= 0; --i) {
        if(aux_boxes[i].first.area() < 1e-6) {
            swap(aux_boxes[i], aux_boxes[r - 1]);
            --r;
        }
    }
    build_tree(l, r, tot, rng, 0);
    aux_boxes.clear();
    aux_boxes_buf.clear();
    ++height;
    have = tot;
}