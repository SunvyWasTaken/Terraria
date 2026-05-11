//
// Created by sunvy on 11/05/2026.
//

#ifndef TERRARIA_BIOMEGEN_H
#define TERRARIA_BIOMEGEN_H

#include <vector>

#include "Biome.h"
#include "LayerWorldGen.h"

struct BiomePoint
{
    Biome biome;
    float temperature;
    float humidity;
};

inline std::vector<BiomePoint> Biomes =
{
    {Biome::Tundra, 0.05f, 0.20f},
    {Biome::Plains, 0.45f, 0.45f},
    {Biome::Forest, 0.45f, 0.85f},
    {Biome::Desert, 0.90f, 0.15f}
};

class BiomeGen : public LayerWorldGen
{
public:
    BiomeGen(int w, int h, int& s);
    void operator()(GameMap& gameMap) override;
    ~BiomeGen() override;
private:
    int width = 0, height = 0;
    int seed = 0;
    float* Humidity = nullptr;
    float* Temperature = nullptr;
};

#endif //TERRARIA_BIOMEGEN_H