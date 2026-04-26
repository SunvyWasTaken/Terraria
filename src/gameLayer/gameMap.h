//
// Created by sunvy on 20/04/2026.
//

#ifndef TERRARIA_GAMEMAP_H
#define TERRARIA_GAMEMAP_H

#include <vector>

#include "blocks.h"

struct GameMap
{
    int width = 0;
    int height = 0;

    std::vector<Block> mapData;
    std::vector<Block> backgroundData;

    void Create(int w, int h);

    Block& GetBlockUnsafe(int x, int y);

    Block* GetBlockSafe(int x, int y);

    Block& GetBackBlockUnsafe(int x, int y);

    Block* GetBackBlockSafe(int x, int y);
};

#endif //TERRARIA_GAMEMAP_H