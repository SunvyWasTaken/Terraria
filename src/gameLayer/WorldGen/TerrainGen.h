//
// Created by sunvy on 11/05/2026.
//

#ifndef TERRARIA_TERRAINGEN_H
#define TERRARIA_TERRAINGEN_H

#include <random>

#include "LayerWorldGen.h"

struct GameMap;

class TerrainGen : public LayerWorldGen
{
public:
    TerrainGen(int weight, int height, std::ranlux24_base& _rng);
    void operator()(GameMap& gameMap) override;
private:
    int w = 0, h = 0;
    std::ranlux24_base& rng;
};

#endif //TERRARIA_TERRAINGEN_H