/*
rng implemented by std
*/
#ifndef RANDOM_MT19937_H
#define RANDOM_MT19937_H
#include "random.hpp"
#include <random>

class RNGMT19937: public RNG {
public:
    RNGMT19937();
    ~RNGMT19937() override = default;
    RNGMT19937(uint seed);

    /// @brief setup rng with seed
    void init(uint seed) override;

    /// @brief return a random interger between [0, 2^32)
    uint rand_uint() override;

    /// @brief return a random float between [0, 1)
    float rand_float() override;

private:
    std::mt19937 rng;
};
#endif
