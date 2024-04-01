/*
base class of random
*/
#ifndef RANDOM_H
#define RANDOM_H

#include "../const.hpp"

class RNG {
public:
    RNG();
    virtual ~RNG() = default;

    /// @brief setup rng with seed
    virtual void init(uint seed);

    /// @brief return a random interger between [0, 2^32)
    virtual uint rand_uint();

    /// @brief return a random float between [0, 1)
    virtual float rand_float();
};
#endif
