//
// Created by sunvy on 23/04/2026.
//

#include <FastNoiseSIMD.h>

#include "worldGenerator.h"

#include <memory>

#include "gameMap.h"
#include "random.h"

#define SINGEN

#ifdef SINGEN

namespace
{
    int w = 900;
    int h = 500;

    struct WormLand
    {
    private:
        enum class Dir : int
        {
            Up = 0,
            Down,
            FastUp,
            FastDown,
            Straight,
            MAX
        };

    public:

        WormLand(int l, int min, int max)
            : layer(l), m_Min(min), m_Max(max)
        {}

        void Process(std::ranlux24_base& rng)
        {
            --m_NbrCountdown;
            if (m_NbrCountdown <= 0)
            {
                m_Direction = static_cast<Dir>(GetRandomInt(rng, 0, static_cast<int>(Dir::MAX) - 1));
                m_NbrCountdown = GetRandomInt(rng, 5, 15);
            }

            switch (m_Direction)
            {
                case Dir::Up:
                    layer += 1;
                    break;
                case Dir::Down:
                    layer -= 1;
                    break;
                case Dir::FastUp:
                    layer += 2;
                    break;
                case Dir::FastDown:
                    layer -= 2;
                    break;
                default:
                    break;
            }

            if (layer < m_Min)
                layer = m_Min;
            else if (layer > m_Max)
                layer = m_Max;
        }

        bool operator==(int i) const
        {
            return layer == i;
        }

        auto operator<=>(int i) const
        {
            return layer <=> i;
        }

        int layer = 0;

    private:
        int m_Min = 0;
        int m_Max = 0;
        Dir m_Direction = Dir::Straight;
        int8_t m_NbrCountdown = 0;
    };

    int operator+(int lhs, const WormLand & rhs)
    {
        return lhs + rhs.layer;
    }

    struct NoiseCaves
    {
        std::unique_ptr<FastNoiseSIMD> NoiseValueGen;
        float* NoiseValue = nullptr;
        std::unique_ptr<FastNoiseSIMD> NoiseSimplexGen;
        float* NoiseSimplex = nullptr;

        explicit NoiseCaves(int seed)
        {
            NoiseValueGen = std::unique_ptr<FastNoiseSIMD>(FastNoiseSIMD::NewFastNoiseSIMD());
            NoiseValueGen->SetSeed(++seed);
            NoiseValueGen->SetNoiseType(FastNoiseSIMD::NoiseType::ValueFractal);
            NoiseValueGen->SetFractalOctaves(3);
            NoiseValueGen->SetFrequency(0.05f);

            NoiseSimplexGen = std::unique_ptr<FastNoiseSIMD>(FastNoiseSIMD::NewFastNoiseSIMD());
            NoiseSimplexGen->SetSeed(++seed);
            NoiseSimplexGen->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
            NoiseSimplexGen->SetFractalOctaves(3);
            NoiseSimplexGen->SetFrequency(0.015f);

            NoiseValue = FastNoiseSIMD::GetEmptySet(w * h);
            NoiseValueGen->FillNoiseSet(NoiseValue, 0, 0, 0, h, w, 1);

            for (int i = 0; i < w * h; ++i)
                NoiseValue[i] = (NoiseValue[i] + 1) / 2;

            NoiseSimplex = FastNoiseSIMD::GetEmptySet(w * h);
            NoiseSimplexGen->FillNoiseSet(NoiseSimplex, 0, 0, 0, h, w, 1);

            for (int i = 0; i < w * h; ++i)
                NoiseSimplex[i] = (NoiseSimplex[i] + 1) / 2;
        }

        ~NoiseCaves()
        {
            FastNoiseSIMD::FreeNoiseSet(NoiseValue);
            FastNoiseSIMD::FreeNoiseSet(NoiseSimplex);
        }

        float operator()(const int x, const int y) const
        {
            const int i = x + y * w;
            return 1.f - (1.f - NoiseValue[i]) * (1.f - NoiseSimplex[i]);
        }
    };
}

void SetWorldSize(int width, int height)
{
    w = width;
    h = height;
}

void GenerateWorld (GameMap& gameMap, int seed)
{
    gameMap.Create(w, h);

    std::ranlux24_base rng(seed);

    WormLand stoneLayer = {GetRandomInt(rng, 115, 200), 115, 200};
    WormLand dirtSize{GetRandomInt(rng, 100, 150), 100, 150};

    const NoiseCaves CaveNoise{seed};

    int desertStart = GetRandomInt(rng, 10, w - 210);
    int desertEnd = desertStart + 100 + GetRandomInt(rng, 0, 100);
    if (desertEnd > w)
        desertEnd = w;

    for (int x = 0; x < w; ++x)
    {
        dirtSize.Process(rng);
        stoneLayer.Process(rng);

        bool InDesert = (x >= desertStart && x <= desertEnd);

        int grassType = Block::grassBlock;
        int dirtType = Block::dirt;
        int stoneType = Block::stone;

        if (InDesert)
        {
            grassType = Block::sand;
            dirtType = Block::sand;
            stoneType = Block::sandStone;
        }

        for (int y = 0; y < h; ++y)
        {
            Block block;

            if (y == dirtSize && y < stoneLayer)
                block.type = grassType;
            else if (y > dirtSize && y < stoneLayer)
                block.type = dirtType;
            else if (y >= stoneLayer)
                block.type = stoneType;

            if (InDesert)
            {
                int desertMid = (desertStart + desertEnd) / 2;
                int desertHalfWidth = (desertEnd - desertStart) / 2;
                int distanceFromDesertMid = std::abs(x - desertMid);
                float desertDistance = 1 - distanceFromDesertMid / float(desertHalfWidth);

                int desertStoneStart = 10 + stoneLayer;
                int desertStoneDepth = 20 + stoneLayer;

                int triangleStoneY = desertStoneStart + desertDistance * desertStoneDepth;

                if (y > triangleStoneY)
                    block.type = Block::stone;
            }

            if (CaveNoise(x, y) < 0.5)
                block.type = Block::air;

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
