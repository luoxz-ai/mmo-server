#include "RandomGet.h"

#include <random>

auto& gen_random()
{
    static thread_local std::random_device rd;
    static thread_local std::mt19937       gen{rd()};
    return gen;
}

uint32_t random_uint32()
{
    auto& gen = gen_random();
    return gen();
}

uint32_t random_uint32_range(uint32_t min, uint32_t max)
{
    std::uniform_int_distribution<uint32_t> _dist(min, max);
    auto&                                   gen = gen_random();
    return _dist(gen);
}

float random_float(float min, float max)
{
    std::uniform_real_distribution<float> _dist(min, max);
    auto&                                 gen = gen_random();
    return _dist(gen);
}

bool random_hit(float op)
{
    std::bernoulli_distribution _dist(op);
    auto&                       gen = gen_random();
    return _dist(gen);
}
