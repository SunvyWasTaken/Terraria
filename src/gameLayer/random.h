//
// Created by sunvy on 22/04/2026.
//

#ifndef TERRARIA_RANDOM_H
#define TERRARIA_RANDOM_H

#include <random>

struct Random
{
    float amplitude = 0.5f;
    float PerlinNoise(int x, int y) const;
};

uint32_t Hash(uint32_t x, uint32_t y, uint32_t seed);

float GetRandomFloat(std::ranlux24_base& rng, float min, float max);

int GetRandomInt(std::ranlux24_base& rng, int min, int max);

// chance should be != 0 or <= 1.0
bool GetRandomBool(std::ranlux24_base& rng, float chance);

#endif //TERRARIA_RANDOM_H