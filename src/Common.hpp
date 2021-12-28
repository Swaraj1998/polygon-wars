#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <vector>
#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <assert.h>

#include "Vec2.hpp"

class RandInt {
    std::default_random_engine re;
    std::uniform_int_distribution<> dist;
public:
    RandInt(int low, int high): dist{low, high} { random_seed(); }
    int operator () () { return dist(re); }
    void seed(int s) { re.seed(s); }
    void random_seed() { re.seed(std::random_device{}()); }
};

#endif