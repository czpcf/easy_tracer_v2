#include <iostream>
#include "../lib/easy_tracer_v2.hpp"

using namespace std;

int main() {
    Vec3 p(1, 2, 3);
    Vec3 d(4, 5, 6);
    Triangle t(Vec3(-100000, -100000, 1000), Vec3(-100000, 100000, 1000), Vec3(1000000, 1000000, 1000));
    RayHit hit;
    cout << hit.get_dis() << endl;
    if(t.inter(Ray(p, d), hit)) {
        Vec3 inter = hit.get_inter(p, d);
        cout << inter.x << ' ' << inter.y << ' ' <<inter.z << endl;
    }
    return 0;
}