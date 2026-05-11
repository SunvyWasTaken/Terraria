//
// Created by sunvy on 11/05/2026.
//

#include "CaveGen.h"

#include "FastNoiseSIMD.h"
#include "gameMap.h"
#include "../random.h"

namespace
{
    int w = 0, h = 0;

    struct Worms
    {
        explicit Worms(GameMap& gameMap, std::ranlux24_base& rng, float PercentChance = 0.1, float x = -1.f, float y = -1.f)
        {
            if (x <= -1)
                x = GetRandomFloat(rng, 10, w - 10);

            if (y <= -1)
                y = GetRandomFloat(rng, 10, h - 10);

            float dirX = GetRandomFloat(rng, -1, 1);
            float dirY = GetRandomFloat(rng, -1, 1);

            int wormLength = GetRandomInt(rng, 100, 200);
            float radius = GetRandomFloat(rng, 2, 5);

            int changeDirectionTime = GetRandomInt(rng, 15, 20);
            for (int j = 0; j < wormLength; ++j)
            {
                --changeDirectionTime;
                if (changeDirectionTime <= 0)
                {
                    if (GetRandomFloat(rng, 0.f, 1.f) < PercentChance)
                        Worms NewWorms{gameMap, rng, PercentChance / 4.f, x, y};

                    changeDirectionTime = GetRandomInt(rng, 10, 20);
                    dirX = GetRandomFloat(rng, -1, 1);
                    dirY = GetRandomFloat(rng, -1, 1);
                }

                x += dirX;
                y += dirY;

                int intRadius = std::ceil(radius);
                for (int ox = -intRadius; ox <= intRadius; ++ox)
                {
                    for (int oy = -intRadius; oy <= intRadius; ++oy)
                    {
                        float distSq = ox * ox + oy * oy;
                        if (distSq <= radius * radius)
                        {
                            int digX = x + ox;
                            int digY = y + oy;

                            if (digY > 0 && digY < h - 1 && digX > 0 && digX < w - 1)
                                gameMap.GetBlockUnsafe(digX, digY).type = Block::air;
                        }
                    }
                }
            }
        }
    };
}

CaveGen::CaveGen(int _w, int _h, int& seed, std::ranlux24_base& _rng)
    : rng(_rng)
{
    w = _w;
    h = _h;

    std::unique_ptr<FastNoiseSIMD> NoiseValueGen = std::unique_ptr<FastNoiseSIMD>(FastNoiseSIMD::NewFastNoiseSIMD());
    NoiseValueGen->SetSeed(++seed);
    NoiseValueGen->SetNoiseType(FastNoiseSIMD::NoiseType::ValueFractal);
    NoiseValueGen->SetFractalOctaves(3);
    NoiseValueGen->SetFrequency(0.05f);

    std::unique_ptr<FastNoiseSIMD> NoiseSimplexGen = std::unique_ptr<FastNoiseSIMD>(FastNoiseSIMD::NewFastNoiseSIMD());
    NoiseSimplexGen->SetSeed(++seed);
    NoiseSimplexGen->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
    NoiseSimplexGen->SetFractalOctaves(3);
    NoiseSimplexGen->SetFrequency(0.015f);

    NoiseValue = FastNoiseSIMD::GetEmptySet(_w * _h);
    NoiseValueGen->FillNoiseSet(NoiseValue, 0, 0, 0, _h, _w, 1);

    for (int i = 0; i < _w * _h; ++i)
        NoiseValue[i] = (NoiseValue[i] + 1) / 2;

    NoiseSimplex = FastNoiseSIMD::GetEmptySet(_w * _h);
    NoiseSimplexGen->FillNoiseSet(NoiseSimplex, 0, 0, 0, _h, _w, 1);

    for (int i = 0; i < _w * _h; ++i)
        NoiseSimplex[i] = (NoiseSimplex[i] + 1) / 2;
}

CaveGen::~CaveGen()
{
    FastNoiseSIMD::FreeNoiseSet(NoiseValue);
    FastNoiseSIMD::FreeNoiseSet(NoiseSimplex);
}

void CaveGen::operator()(GameMap &gameMap)
{
    auto get = [&](int x, int y)
    {
        const int i = x + y * w;
        return 1.f - (1.f - NoiseValue[i]) * (1.f - NoiseSimplex[i]);
    };

    for (int x = 0; x < w; ++x)
    {
        for (int y = 0; y < h; ++y)
        {
            if (get(x, y) <= 0.5f)
                gameMap.GetBlockUnsafe(x, y).type = Block::air;
        }
    }

    const int NbrWorms = GetRandomInt(rng, 20, 30);

    for (int i = 0; i < NbrWorms; ++i)
    {
        Worms worm{gameMap, rng};
    }
}
