#include <iostream>
#include "../lib/easy_tracer_v2.hpp"

using namespace std;

int main() {
    SamplerLambertian x;
    RNGMT19937 rng(123);
    for(int i = 0; i < 4; ++i) {
        Vec3 out;
        float pdf;
        x.sample_out(&rng, Vec3(0, 0, 1), Vec3(0, 0, 1), out, pdf);
        cout << out.x << ' ' << out.y << ' ' << out.z << ' ' << pdf << endl;
    }
    return 0;
}