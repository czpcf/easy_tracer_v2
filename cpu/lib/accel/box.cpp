#include "box.hpp"
#include "../const.hpp"
#include <errno.h>
#include <iostream>

static float EPS = 1e-6;

Box::Box() {
}

const Vec3 &Box::lower() const {
    return _lower;
}

const Vec3 &Box::upper() const {
    return _upper;
}

const Vec3 &Box::mid() const {
    return _mid;
}

Box::Box(const Vec3 &p1, const Vec3 &p2) {
    _lower = min(p1, p2);
    _upper = max(p1, p2);
    _mid = (_lower + _upper) * 0.5f;
}

void Box::add(const Vec3 &p) {
    _lower = min(_lower, p);
    _upper = max(_upper, p);
    _mid = (_lower + _upper) * 0.5f;
}

void Box::add(const Box &b) {
    _lower = min(_lower, b._lower);
    _upper = max(_upper, b._upper);
    _mid = (_lower + _upper) * 0.5f;
}

bool Box::inside(const Vec3 &p) {
    return _lower.x <= p.x && p.x <= _upper.x &&
           _lower.y <= p.y && p.y <= _upper.y &&
           _lower.z <= p.z && p.z <= _upper.z;
}

// TODO: be more efficient
bool Box::inter_if(const Vec3 &p, const Vec3 &d) {
    auto check = [&](float dx, float dy, float dz, float px, float py, float pz,
        float lower_x, float upper_x, float lower_y, float upper_y, float lower_z, float upper_z) {
        float dis;
        if(dx >= EPS) {
            if(lower_x <= px && px <= upper_x) {
                dis = (upper_x - px) / dx;
            } else {
                dis = (lower_x - px) / dx;
            }
        } else if(dx <= -EPS) {
            if(lower_x <= px && px <= upper_x) {
                dis = (lower_x - px) / dx;
            } else {
                dis = (upper_x - px) / dx;
            }
        } else {
            return false;
        }
        py += dis * dy;
        pz += dis * dz;
        if(dis < 0) { // be careful
            return false;
        }
        if(!(lower_y <= py && py <= upper_y &&
             lower_z <= pz && pz <= upper_z)) {
            return false;
        }
        return true;
    };
    if(check(d.x, d.y, d.z, p.x, p.y, p.z, _lower.x, _upper.x, _lower.y, _upper.y, _lower.z, _upper.z) ||
       check(d.y, d.x, d.z, p.y, p.x, p.z, _lower.y, _upper.y, _lower.x, _upper.x, _lower.z, _upper.z) ||
       check(d.z, d.x, d.y, p.z, p.x, p.y, _lower.z, _upper.z, _lower.x, _upper.x, _lower.y, _upper.y)) {
        return true;
    }
    return false;
}

// TODO: be more efficient
float Box::inter_normed(const Vec3 &p, const Vec3 &d) {
    float dis = 1e38;
    /*
    auto check = [&](float dx, float dy, float dz, float px, float py, float pz,
        float lower_x, float upper_x, float lower_y, float upper_y, float lower_z, float upper_z) {
        float ndis;
        if(dx >= EPS) {
            if(lower_x <= px && px <= upper_x) {
                ndis = (upper_x - px) / dx;
            } else {
                ndis = (lower_x - px) / dx;
            }
        } else if(dx <= -EPS) {
            if(lower_x <= px && px <= upper_x) {
                ndis = (lower_x - px) / dx;
            } else {
                ndis = (upper_x - px) / dx;
            }
        } else {
            return;
        }
        if(ndis < 0 || ndis >= dis) { // be careful
            return;
        }
        py += ndis * dy; // be careful
        pz += ndis * dz;
        if(!(lower_y <= py && py <= upper_y &&
             lower_z <= pz && pz <= upper_z)) {
            return;
        }
        dis = ndis;
    };
    check(d.x, d.y, d.z, p.x, p.y, p.z, lower.x, upper.x, lower.y, upper.y, lower.z, upper.z);
    check(d.y, d.x, d.z, p.y, p.x, p.z, lower.y, upper.y, lower.x, upper.x, lower.z, upper.z);
    check(d.z, d.x, d.y, p.z, p.x, p.y, lower.z, upper.z, lower.x, upper.x, lower.y, upper.y);
    */
    
    // x y z
    float ndis = -1;
    if(d.x >= EPS) {
        if(_lower.x <= p.x && p.x <= _upper.x) {
            ndis = (_upper.x - p.x) / d.x;
        } else {
            ndis = (_lower.x - p.x) / d.x;
        }
    } else if(d.x <= -EPS) {
        if(_lower.x <= p.x && p.x <= _upper.x) {
            ndis = (_lower.x - p.x) / d.x;
        } else {
            ndis = (_upper.x - p.x) / d.x;
        }
    }
    if(ndis > 0 && ndis < dis) { // be careful
        float py = p.y + ndis * d.y; // be careful
        float pz = p.z + ndis * d.z;
        if(_lower.y <= py && py <= _upper.y &&
           _lower.z <= pz && pz <= _upper.z) {
            dis = ndis;
        }
    }

    // y x z
    ndis = -1;
    if(d.y >= EPS) {
        if(_lower.y <= p.y && p.y <= _upper.y) {
            ndis = (_upper.y - p.y) / d.y;
        } else {
            ndis = (_lower.y - p.y) / d.y;
        }
    } else if(d.y <= -EPS) {
        if(_lower.y <= p.y && p.y <= _upper.y) {
            ndis = (_lower.y - p.y) / d.y;
        } else {
            ndis = (_upper.y - p.y) / d.y;
        }
    }
    if(ndis > 0 && ndis < dis) { // be careful
        float px = p.x + ndis * d.x; // be careful
        float pz = p.z + ndis * d.z;
        if(_lower.x <= px && px <= _upper.x &&
           _lower.z <= pz && pz <= _upper.z) {
            dis = ndis;
        }
    }

    // z x y
    ndis = -1;
    if(d.z >= EPS) {
        if(_lower.z <= p.z && p.z <= _upper.z) {
            ndis = (_upper.z - p.z) / d.z;
        } else {
            ndis = (_lower.z - p.z) / d.z;
        }
    } else if(d.z <= -EPS) {
        if(_lower.z <= p.z && p.z <= _upper.z) {
            ndis = (_lower.z - p.z) / d.z;
        } else {
            ndis = (_upper.z - p.z) / d.z;
        }
    }
    if(ndis > 0 && ndis < dis) { // be careful
        float px = p.x + ndis * d.x; // be careful
        float py = p.y + ndis * d.y;
        if(_lower.x <= px && px <= _upper.x &&
           _lower.y <= py && py <= _upper.y) {
            dis = ndis;
        }
    }
    return dis;
}

// TODO: be more efficient
float Box::inter_squared(const Vec3 &p, const Vec3 &d) {
    float dis = 1e38;
    /*
    auto check = [&](float dx, float dy, float dz, float px, float py, float pz,
        float lower_x, float upper_x, float lower_y, float upper_y, float lower_z, float upper_z) {
        float ndis;
        if(dx >= EPS) {
            if(lower_x <= px && px <= upper_x) {
                ndis = (upper_x - px) / dx;
            } else {
                ndis = (lower_x - px) / dx;
            }
        } else if(dx <= -EPS) {
            if(lower_x <= px && px <= upper_x) {
                ndis = (lower_x - px) / dx;
            } else {
                ndis = (upper_x - px) / dx;
            }
        } else {
            return;
        }
        if(ndis < 0) { // be careful
            return;
        }
        py += ndis * dy; // be careful
        pz += ndis * dz;
        if(!(lower_y <= py && py <= upper_y &&
             lower_z <= pz && pz <= upper_z)) {
            return;
        }
        dis = std::min(dis, ndis);
    };
    check(d.x, d.y, d.z, p.x, p.y, p.z, lower.x, upper.x, lower.y, upper.y, lower.z, upper.z);
    check(d.y, d.x, d.z, p.y, p.x, p.z, lower.y, upper.y, lower.x, upper.x, lower.z, upper.z);
    check(d.z, d.x, d.y, p.z, p.x, p.y, lower.z, upper.z, lower.x, upper.x, lower.y, upper.y);
    */
    
    // x y z
    float ndis = -1;
    if(d.x >= EPS) {
        if(_lower.x <= p.x && p.x <= _upper.x) {
            ndis = (_upper.x - p.x) / d.x;
        } else {
            ndis = (_lower.x - p.x) / d.x;
        }
    } else if(d.x <= -EPS) {
        if(_lower.x <= p.x && p.x <= _upper.x) {
            ndis = (_lower.x - p.x) / d.x;
        } else {
            ndis = (_upper.x - p.x) / d.x;
        }
    }
    if(ndis > 0 && ndis < dis) { // be careful
        float py = p.y + ndis * d.y; // be careful
        float pz = p.z + ndis * d.z;
        if(_lower.y <= py && py <= _upper.y &&
           _lower.z <= pz && pz <= _upper.z) {
            dis = ndis;
        }
    }

    // y x z
    ndis = -1;
    if(d.y >= EPS) {
        if(_lower.y <= p.y && p.y <= _upper.y) {
            ndis = (_upper.y - p.y) / d.y;
        } else {
            ndis = (_lower.y - p.y) / d.y;
        }
    } else if(d.y <= -EPS) {
        if(_lower.y <= p.y && p.y <= _upper.y) {
            ndis = (_lower.y - p.y) / d.y;
        } else {
            ndis = (_upper.y - p.y) / d.y;
        }
    }
    if(ndis > 0 && ndis < dis) { // be careful
        float px = p.x + ndis * d.x; // be careful
        float pz = p.z + ndis * d.z;
        if(_lower.x <= px && px <= _upper.x &&
           _lower.z <= pz && pz <= _upper.z) {
            dis = ndis;
        }
    }

    // z x y
    ndis = -1;
    if(d.z >= EPS) {
        if(_lower.z <= p.z && p.z <= _upper.z) {
            ndis = (_upper.z - p.z) / d.z;
        } else {
            ndis = (_lower.z - p.z) / d.z;
        }
    } else if(d.z <= -EPS) {
        if(_lower.z <= p.z && p.z <= _upper.z) {
            ndis = (_lower.z - p.z) / d.z;
        } else {
            ndis = (_upper.z - p.z) / d.z;
        }
    }
    if(ndis > 0 && ndis < dis) { // be careful
        float px = p.x + ndis * d.x; // be careful
        float py = p.y + ndis * d.y;
        if(_lower.x <= px && px <= _upper.x &&
           _lower.y <= py && py <= _upper.y) {
            dis = ndis;
        }
    }
    return dis * dis * d.square();
}

// TODO: be more efficient
float Box::inter_squared_normed(const Vec3 &p, const Vec3 &d) {
    float dis = 1e38;
    /*
    auto check = [&](float dx, float dy, float dz, float px, float py, float pz,
        float lower_x, float upper_x, float lower_y, float upper_y, float lower_z, float upper_z) {
        float ndis;
        if(dx >= EPS) {
            if(lower_x <= px && px <= upper_x) {
                ndis = (upper_x - px) / dx;
            } else {
                ndis = (lower_x - px) / dx;
            }
        } else if(dx <= -EPS) {
            if(lower_x <= px && px <= upper_x) {
                ndis = (lower_x - px) / dx;
            } else {
                ndis = (upper_x - px) / dx;
            }
        } else {
            return;
        }
        if(ndis < 0 || ndis >= dis) { // be careful
            return;
        }
        py += ndis * dy; // be careful
        pz += ndis * dz;
        if(!(lower_y <= py && py <= upper_y &&
             lower_z <= pz && pz <= upper_z)) {
            return;
        }
        dis = ndis;
    };
    check(d.x, d.y, d.z, p.x, p.y, p.z, lower.x, upper.x, lower.y, upper.y, lower.z, upper.z);
    check(d.y, d.x, d.z, p.y, p.x, p.z, lower.y, upper.y, lower.x, upper.x, lower.z, upper.z);
    check(d.z, d.x, d.y, p.z, p.x, p.y, lower.z, upper.z, lower.x, upper.x, lower.y, upper.y);
    */
    
    // x y z
    float ndis = -1;
    if(d.x >= EPS) {
        if(_lower.x <= p.x && p.x <= _upper.x) {
            ndis = (_upper.x - p.x) / d.x;
        } else {
            ndis = (_lower.x - p.x) / d.x;
        }
    } else if(d.x <= -EPS) {
        if(_lower.x <= p.x && p.x <= _upper.x) {
            ndis = (_lower.x - p.x) / d.x;
        } else {
            ndis = (_upper.x - p.x) / d.x;
        }
    }
    if(ndis > 0 && ndis < dis) { // be careful
        float py = p.y + ndis * d.y; // be careful
        float pz = p.z + ndis * d.z;
        if(_lower.y <= py && py <= _upper.y &&
           _lower.z <= pz && pz <= _upper.z) {
            dis = ndis;
        }
    }

    // y x z
    ndis = -1;
    if(d.y >= EPS) {
        if(_lower.y <= p.y && p.y <= _upper.y) {
            ndis = (_upper.y - p.y) / d.y;
        } else {
            ndis = (_lower.y - p.y) / d.y;
        }
    } else if(d.y <= -EPS) {
        if(_lower.y <= p.y && p.y <= _upper.y) {
            ndis = (_lower.y - p.y) / d.y;
        } else {
            ndis = (_upper.y - p.y) / d.y;
        }
    }
    if(ndis > 0 && ndis < dis) { // be careful
        float px = p.x + ndis * d.x; // be careful
        float pz = p.z + ndis * d.z;
        if(_lower.x <= px && px <= _upper.x &&
           _lower.z <= pz && pz <= _upper.z) {
            dis = ndis;
        }
    }

    // z x y
    ndis = -1;
    if(d.z >= EPS) {
        if(_lower.z <= p.z && p.z <= _upper.z) {
            ndis = (_upper.z - p.z) / d.z;
        } else {
            ndis = (_lower.z - p.z) / d.z;
        }
    } else if(d.z <= -EPS) {
        if(_lower.z <= p.z && p.z <= _upper.z) {
            ndis = (_lower.z - p.z) / d.z;
        } else {
            ndis = (_upper.z - p.z) / d.z;
        }
    }
    if(ndis > 0 && ndis < dis) { // be careful
        float px = p.x + ndis * d.x; // be careful
        float py = p.y + ndis * d.y;
        if(_lower.x <= px && px <= _upper.x &&
           _lower.y <= py && py <= _upper.y) {
            dis = ndis;
        }
    }
    return dis * dis;
}

// TODO: be more efficient
float Box::inter_squared_normed_outside(const Vec3 &p, const Vec3 &d) {
    float dis = 1e38;
    // x y z
    float ndis = -1;
    if(d.x >= EPS) {
        ndis = (_lower.x - p.x) / d.x;
    } else if(d.x <= -EPS) {
        ndis = (_upper.x - p.x) / d.x;
    }
    if(ndis > 0 && ndis < dis) { // be careful
        float py = p.y + ndis * d.y; // be careful
        float pz = p.z + ndis * d.z;
        if(_lower.y <= py && py <= _upper.y &&
           _lower.z <= pz && pz <= _upper.z) {
            dis = ndis;
        }
    }

    // y x z
    ndis = -1;
    if(d.y >= EPS) {
        ndis = (_lower.y - p.y) / d.y;
    } else if(d.y <= -EPS) {
        ndis = (_upper.y - p.y) / d.y;
    }
    if(ndis > 0 && ndis < dis) { // be careful
        float px = p.x + ndis * d.x; // be careful
        float pz = p.z + ndis * d.z;
        if(_lower.x <= px && px <= _upper.x &&
           _lower.z <= pz && pz <= _upper.z) {
            dis = ndis;
        }
    }

    // z x y
    ndis = -1;
    if(d.z >= EPS) {
        ndis = (_lower.z - p.z) / d.z;
    } else if(d.z <= -EPS) {
        ndis = (_upper.z - p.z) / d.z;
    }
    if(ndis > 0 && ndis < dis) { // be careful
        float px = p.x + ndis * d.x; // be careful
        float py = p.y + ndis * d.y;
        if(_lower.x <= px && px <= _upper.x &&
           _lower.y <= py && py <= _upper.y) {
            dis = ndis;
        }
    }
    return dis * dis;
}

// TODO: be more efficient
float Box::inter_normed_outside(const Vec3 &p, const Vec3 &d) {
    float dis = 1e38;
    // x y z
    float ndis = -1;
    if(d.x >= EPS) {
        ndis = (_lower.x - p.x) / d.x;
    } else if(d.x <= -EPS) {
        ndis = (_upper.x - p.x) / d.x;
    }
    if(ndis > 0 && ndis < dis) { // be careful
        float py = p.y + ndis * d.y; // be careful
        float pz = p.z + ndis * d.z;
        if(_lower.y <= py && py <= _upper.y &&
           _lower.z <= pz && pz <= _upper.z) {
            dis = ndis;
        }
    }

    // y x z
    ndis = -1;
    if(d.y >= EPS) {
        ndis = (_lower.y - p.y) / d.y;
    } else if(d.y <= -EPS) {
        ndis = (_upper.y - p.y) / d.y;
    }
    if(ndis > 0 && ndis < dis) { // be careful
        float px = p.x + ndis * d.x; // be careful
        float pz = p.z + ndis * d.z;
        if(_lower.x <= px && px <= _upper.x &&
           _lower.z <= pz && pz <= _upper.z) {
            dis = ndis;
        }
    }

    // z x y
    ndis = -1;
    if(d.z >= EPS) {
        ndis = (_lower.z - p.z) / d.z;
    } else if(d.z <= -EPS) {
        ndis = (_upper.z - p.z) / d.z;
    }
    if(ndis > 0 && ndis < dis) { // be careful
        float px = p.x + ndis * d.x; // be careful
        float py = p.y + ndis * d.y;
        if(_lower.x <= px && px <= _upper.x &&
           _lower.y <= py && py <= _upper.y) {
            dis = ndis;
        }
    }
    return dis;
}

// TODO: be more efficient
bool Box::check_normed_outsideX(const Vec3 &p, const Vec3 &d, float dis) {
    // x y z
    float ndis = -1;
    if(d.x >= EPS) {
        ndis = (_lower.x - p.x) / d.x;
    } else if(d.x <= -EPS) {
        ndis = (_upper.x - p.x) / d.x;
    }
    if(ndis > 0 && ndis * d.x < dis) { // be careful
        float py = p.y + ndis * d.y; // be careful
        float pz = p.z + ndis * d.z;
        if(_lower.y <= py && py <= _upper.y &&
           _lower.z <= pz && pz <= _upper.z) {
            return true;
        }
    }

    // y x z
    ndis = -1;
    if(d.y >= EPS) {
        ndis = (_lower.y - p.y) / d.y;
    } else if(d.y <= -EPS) {
        ndis = (_upper.y - p.y) / d.y;
    }
    if(ndis > 0 && ndis * d.x < dis) { // be careful
        float px = p.x + ndis * d.x; // be careful
        float pz = p.z + ndis * d.z;
        if(_lower.x <= px && px <= _upper.x &&
           _lower.z <= pz && pz <= _upper.z) {
            return true;
        }
    }

    // z x y
    ndis = -1;
    if(d.z >= EPS) {
        ndis = (_lower.z - p.z) / d.z;
    } else if(d.z <= -EPS) {
        ndis = (_upper.z - p.z) / d.z;
    }
    if(ndis > 0 && ndis * d.x < dis) { // be careful
        float px = p.x + ndis * d.x; // be careful
        float py = p.y + ndis * d.y;
        if(_lower.x <= px && px <= _upper.x &&
           _lower.y <= py && py <= _upper.y) {
            return true;
        }
    }
    return false;
}

// TODO: be more efficient
std::pair<bool, float> Box::inter(const Vec3 &p, const Vec3 &d) {
    float dis1, dis2, dis3;
    auto check = [&](float& dis,
        float dx, float dy, float dz, float px, float py, float pz,
        float lower_x, float upper_x, float lower_y, float upper_y, float lower_z, float upper_z) {

        if(dx >= EPS) {
            if(lower_x <= px && px <= upper_x) {
                dis = (upper_x - px) / dx;
            } else {
                dis = (lower_x - px) / dx;
            }
        } else if(dx <= -EPS) {
            if(lower_x <= px && px <= upper_x) {
                dis = (lower_x - px) / dx;
            } else {
                dis = (upper_x - px) / dx;
            }
        } else {
            dis = 1e38;
            return;
        }
        if(dis < 0) { // be careful
            dis = 1e38;
        }
        py += dy * dis;
        pz += dz * dis;
        if(!(lower_y <= py && py <= upper_y &&
             lower_z <= pz && pz <= upper_z)) {
            dis = 1e38;
        }
    };
    check(dis1, d.x, d.y, d.z, p.x, p.y, p.z, _lower.x, _upper.x, _lower.y, _upper.y, _lower.z, _upper.z);
    check(dis2, d.y, d.x, d.z, p.y, p.x, p.z, _lower.y, _upper.y, _lower.x, _upper.x, _lower.z, _upper.z);
    check(dis3, d.z, d.x, d.y, p.z, p.x, p.y, _lower.z, _upper.z, _lower.x, _upper.x, _lower.y, _upper.y);
    float dis = 1e38;
    if(dis1 < dis) {
        dis = dis1;
    }
    if(dis2 < dis) {
        dis = dis2;
    }
    if(dis3 < dis) {
        dis = dis3;
    }
    if(dis > 1e37) {
        return std::make_pair(false, 0.0f);
    }
    return std::make_pair(true, dis);
}

float Box::area() {
    float dx = _upper.x - _lower.x;
    float dy = _upper.y - _lower.y;
    float dz = _upper.z - _lower.z;
    return (dx * dy + dx * dz + dy * dz) * 2.0f;
}

float Box::half_area() {
    float dx = _upper.x - _lower.x;
    float dy = _upper.y - _lower.y;
    float dz = _upper.z - _lower.z;
    return dx * dy + dx * dz + dy * dz;
}
