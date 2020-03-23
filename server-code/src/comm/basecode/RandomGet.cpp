#include "RandomGet.h"

#include <random>

thread_local std::random_device  rd;
thread_local static std::mt19937 gen(rd());

uint32_t random_uint32()
{
    return gen();
}

uint32_t random_uint32_range(uint32_t min, uint32_t max)
{
    std::uniform_int_distribution<uint32_t> _dist(min, max);
    return _dist(gen);
}

float random_float(float min, float max)
{
    std::uniform_real_distribution<float> _dist(min, max);
    return _dist(gen);
}

bool random_hit(float op)
{
    std::bernoulli_distribution _dist(op);
    return _dist(gen);
}
