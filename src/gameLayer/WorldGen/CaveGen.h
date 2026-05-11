//
// Created by sunvy on 11/05/2026.
//

#ifndef TERRARIA_CAVEGEN_H
#define TERRARIA_CAVEGEN_H

#include <memory>
#include <random>

#include "LayerWorldGen.h"

class FastNoiseSIMD;

class CaveGen : public LayerWorldGen
{
public:
    CaveGen(int _w, int _h, int& seed, std::ranlux24_base& _rng);
    ~CaveGen() override;
    void operator()(GameMap& gameMap) override;
private:
    std::ranlux24_base& rng;
    float* NoiseValue = nullptr;
    float* NoiseSimplex = nullptr;
};

#endif //TERRARIA_CAVEGEN_H