//
// Created by sunvy on 20/04/2026.
//

#include "gameMap.h"

#include <assert.h>

void GameMap::Create(int w, int h)
{
    *this = {}; // reset all the data

    width = w;
    height = h;

    mapData.resize(w * h);
    for (auto& b : mapData)
        b = {}; // clear all block data

    backgroundData.resize(w * h);
    for (auto& b : backgroundData)
        b = {};
}

Block& GameMap::GetBlockUnsafe(int x, int y)
{
    assert(mapData.size() == width * height);
    assert(x >= 0 && y >= 0 && x < width && y < height);
    return mapData[y * width + x];
}

Block* GameMap::GetBlockSafe(int x, int y)
{
    assert(mapData.size() == width * height);
    if (x < 0 || y < 0 || x >= width || y >= height)
        return nullptr;

    return &mapData[y * width + x];
}

Block& GameMap::GetBackBlockUnsafe(int x, int y)
{
    assert(mapData.size() == width * height);
    assert(x >= 0 && y >= 0 && x < width && y < height);
    return backgroundData[y * width + x];
}

Block* GameMap::GetBackBlockSafe(int x, int y)
{
    assert(mapData.size() == width * height);
    if (x < 0 || y < 0 || x >= width || y >= height)
        return nullptr;

    return &backgroundData[y * width + x];
}
