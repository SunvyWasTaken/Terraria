//
// Created by sunvy on 23/04/2026.
//

#include <FastNoiseSIMD.h>

#include "worldGenerator.h"

#include <memory>

#include "gameMap.h"
#include "random.h"

#ifdef SINGEN
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
#else

void GenerateWorld (GameMap& gameMap, int seed)
{
    const int w = 900;
    const int h = 500;

    gameMap.Create(w, h);

    // Create the noise.
    std::unique_ptr<FastNoiseSIMD> dirtNoiseGenerator(FastNoiseSIMD::NewFastNoiseSIMD());
    std::unique_ptr<FastNoiseSIMD> stoneNoiseGenerator(FastNoiseSIMD::NewFastNoiseSIMD());

    // Set Seed
    dirtNoiseGenerator->SetSeed(seed++);
    stoneNoiseGenerator->SetSeed(seed++);

    // Set parameter
    dirtNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
    dirtNoiseGenerator->SetFractalOctaves(2);
    dirtNoiseGenerator->SetFrequency(0.02);

    stoneNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
    stoneNoiseGenerator->SetFractalOctaves(4);
    stoneNoiseGenerator->SetFrequency(0.01);

    /* Generate Noise with value.
     * It's going to gen a 1D noise to represent the terrain height.
     */
    float* dirtNoise = FastNoiseSIMD::GetEmptySet(w); // SIMD use some spec alloc to be fast (wich I'm not using cuz linux -_-)
    float* stoneNoise = FastNoiseSIMD::GetEmptySet(w);

    dirtNoiseGenerator->FillNoiseSet(dirtNoise, 0, 0, 0, w, 1, 1);
    stoneNoiseGenerator->FillNoiseSet(stoneNoise, 0, 0, 0, w, 1, 1);

    for (int i = 0; i < w; ++i)
    {
        dirtNoise[i] = (dirtNoise[i] + 1) / 2;
        stoneNoise[i] = (stoneNoise[i] + 1) / 2;
    }

    int dirtOffsetStart = -5;
    int dirtOffsetEnd = 35;

    int stoneHeightStart = 80;
    int stoneHeightEnd = 170;

    for (int x = 0; x < w; ++x)
    {
        int stoneHeight = stoneHeightStart + (stoneHeightEnd - stoneHeightStart) * stoneNoise[x];
        int dirtHeight = dirtOffsetStart + (dirtOffsetEnd - dirtOffsetStart) * dirtNoise[x];
        dirtHeight = stoneHeight - dirtHeight;

        for (int y = 0; y < h; ++y)
        {
            Block block;
            if (y > dirtHeight)
                block.type = Block::dirt;
            if (y == dirtHeight)
                block.type = Block::grassBlock;
            if (y >= stoneHeight)
                block.type = Block::stone;

            gameMap.GetBlockUnsafe(x, y) = block;
        }
    }

    FastNoiseSIMD::FreeNoiseSet(dirtNoise);
    FastNoiseSIMD::FreeNoiseSet(stoneNoise);
}

#endif
