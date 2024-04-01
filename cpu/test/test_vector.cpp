#include <iostream>
#include "../lib/easy_tracer_v2.hpp"

using namespace std;

int main() {
    Vec3 a(1, 2, 3);
    Vec3 b(0, 1, 1);
    Vec3 c = a + b;
    cout << c.x << ' ' << c.y << ' ' << c.z << endl;
}