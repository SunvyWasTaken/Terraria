//
// Created by sunvy on 23/04/2026.
//

#include "worldGenerator.h"

#include "gameMap.h"
#include "random.h"

#define NBRLAYER 32

float Fbm(int val, std::vector<float>& frequencies, std::vector<float>& amplitudes)
{
    float value = 0;
    for (int i = 0; i < frequencies.size(); ++i)
    {
        value += sin(val * frequencies[i]) * amplitudes[i];
    }
    return value;
}

float FbmCos(int val, std::vector<float>& frequencies, std::vector<float>& amplitudes)
{
    float value = 0;
    for (int i = 0; i < frequencies.size(); ++i)
    {
        value += cos(val * frequencies[i]) * amplitudes[i];
    }
    return value;
}

void GenerateWorld (GameMap& gameMap, int seed)
{
    const int w = 900;
    const int h = 500;

    gameMap.Create(w, h);

    int stoneSize = 380;
    int dirtSize = 50;

    std::ranlux24_base rng(seed);

    std::vector<float> frequencies;
    std::vector<float> amplitudes;
    frequencies.reserve(NBRLAYER);
    amplitudes.reserve(NBRLAYER);

    for (int i = 0; i < NBRLAYER; ++i)
        frequencies.emplace_back(GetRandomFloat(rng, 0.001, 0.09));

    for (int i = 0; i < NBRLAYER; ++i)
        amplitudes.emplace_back(GetRandomFloat(rng, 1, 1.5));

    for (int x = 0; x < w; ++x)
    {
        dirtSize = 50 + Fbm(x, frequencies, amplitudes);
        stoneSize = 380 + Fbm(x, frequencies, amplitudes);

        for (int y = 0; y < h; ++y)
        {
            Block block;
            if (y < h - (dirtSize + stoneSize))
            {}
            else if (y == h - (dirtSize + stoneSize))
                block.type = Block::grassBlock;
            else if (y < h - stoneSize)
                block.type = Block::dirt;
            else
            {
                block.type = Block::stone;

                if (GetRandomBool(rng, 0.1))
                {
                    block.type = Block::gold;
                }
            }

            gameMap.GetBlockUnsafe(x, y) = block;
        }
    }
}
