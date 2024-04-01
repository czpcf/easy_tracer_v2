#include "random_mt19937.hpp"

RNGMT19937::RNGMT19937() {
    rng.seed(0);
}

RNGMT19937::RNGMT19937(uint seed){
    rng.seed(seed);
}

void RNGMT19937::init(uint seed) {
    rng.seed(seed);
}

uint RNGMT19937::rand_uint() {
    return rng() & 0xffffffff;
}

float RNGMT19937::rand_float() {
    return float(rng() & 0x7fffffff) / float(0x80000000);
}
