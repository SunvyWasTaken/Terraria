//
// Created by sunvy on 11/05/2026.
//

#include "TerrainGen.h"

#include "Biome.h"
#include "../blocks.h"
#include "../gameMap.h"
#include "../random.h"

namespace
{
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
}

TerrainGen::TerrainGen(int weight, int height, std::ranlux24_base &_rng)
    : w(weight)
    , h(height)
    , rng(_rng)
{
}

void TerrainGen::operator()(GameMap &gameMap)
{
    WormLand stoneLayer = {GetRandomInt(rng, 115, 200), 115, 200};
    WormLand dirtSize{GetRandomInt(rng, 100, 150), 100, 150};

    for (int x = 0; x < w; ++x)
    {
        dirtSize.Process(rng);
        stoneLayer.Process(rng);

        int grassType = Block::grassBlock;
        int dirtType = Block::dirt;
        int stoneType = Block::stone;

        for (int y = 0; y < h; ++y)
        {
            Block block;

            Biome b = gameMap.biomeData[y * w + x];
            switch (b)
            {
                case Biome::Plains:
                {
                    grassType = Block::grassBlock;
                    dirtType = Block::dirt;
                    stoneType = Block::stone;
                    break;
                }
                case Biome::Desert:
                {
                    grassType = Block::sand;
                    dirtType = Block::sand;
                    stoneType = Block::sandStone;
                    break;
                }
                case Biome::Forest:
                {
                    grassType = Block::grassBlock;
                    dirtType = Block::dirt;
                    stoneType = Block::stone;
                    break;
                }
                    case Biome::Tundra:
                {
                    grassType = Block::snow;
                    dirtType = Block::snow;
                    stoneType = Block::ice;
                    break;
                }
                default:
                {
                    break;
                }
            }

            if (y == dirtSize && y < stoneLayer)
                block.type = grassType;
            else if (y > dirtSize && y < stoneLayer)
                block.type = dirtType;
            else if (y >= stoneLayer)
                block.type = stoneType;

            gameMap.GetBlockUnsafe(x, y) = block;
        }
    }
}
