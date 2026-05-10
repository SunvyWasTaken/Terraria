//
// Created by sunvy on 10/05/2026.
//

#ifndef TERRARIA_STRUCTURE_H
#define TERRARIA_STRUCTURE_H
#include <vector>
#include <raylib.h>

#include "blocks.h"
#include "gameMap.h"

struct structure
{
    int width = 0;
    int height = 0;

    std::vector<Block> mapData;

    void Create(int w, int h);

    Block &GetBlockUnsafe(int x, int y);

    Block *GetBlockSafe(int x, int y);

    void copyFromMap(GameMap &map, Vector2 start, Vector2 end);

    void pasteIntoMap(GameMap &map, const Vector2& start);
};

#endif //TERRARIA_STRUCTURE_H