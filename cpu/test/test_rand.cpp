#include <iostream>
#include "../lib/easy_tracer_v2.hpp"

using namespace std;

int main() {
    RNGMT19937 rng(123);

    cout << rng.rand_float() << endl;
    cout << rng.rand_float() << endl;
    cout << rng.rand_float() << endl;

    int buc[16];
    for(int i = 0; i < 16; ++i) {
        buc[i] = 0;
    }
    for(int i = 0; i < 1024; ++i) {
        ++buc[rng.rand_uint() % 16];
    }
    for(int i = 0; i < 16; ++i) {
        cout << buc[i] << ' ';
    }
    cout << endl;
    return 0;
}