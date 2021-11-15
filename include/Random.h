#ifndef RANDOM_H
#define RANDOM_H
#pragma once

#include <random>

/** global variable for random engine */
namespace Random {
    // random engine
    extern std::default_random_engine Engine;

    // set up random seed for different processors
    void set_seed(const int &rank);

    // set up fixed seed
    void set_seed_fix(const int &seed);
}


#endif // RANDOM_H