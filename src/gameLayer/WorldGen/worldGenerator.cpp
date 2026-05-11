//
// Created by sunvy on 23/04/2026.
//

#include <FastNoiseSIMD.h>

#include "worldGenerator.h"

#include <memory>

#include "BiomeGen.h"
#include "CaveGen.h"
#include "LayerWorldGen.h"
#include "TerrainGen.h"
#include "../gameMap.h"
#include "../random.h"

//#define SINGEN

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

        std::strong_ordering operator<=>(int i) const
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

    const int NbrWorms = GetRandomInt(rng, 20, 30);

    for (int i = 0; i < NbrWorms; ++i)
    {
        Worms worm{gameMap, rng};
    }
}
#else

void GenerateWorld (GameMap& gameMap, int seed)
{
    const int w = 900;
    const int h = 500;

    gameMap.Create(w, h);

    std::ranlux24_base rng(seed);

    std::vector<std::unique_ptr<LayerWorldGen>> layers;

    layers.emplace_back(std::make_unique<BiomeGen>(w, h, seed));
    layers.emplace_back(std::make_unique<TerrainGen>(w, h, rng));
    layers.emplace_back(std::make_unique<CaveGen>(w, h, seed, rng));

    for (auto& layer : layers)
    {
        (*layer)(gameMap);
    }
}

#endif
