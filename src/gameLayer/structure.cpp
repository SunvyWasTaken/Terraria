//
// Created by sunvy on 10/05/2026.
//

#include "structure.h"

#include <cassert>

void structure::Create(int w, int h)
{
    *this = {}; // reset all the data

    width = w;
    height = h;

    mapData.resize(w * h);
    for (auto& b : mapData)
        b = {}; // clear all block data
}

Block & structure::GetBlockUnsafe(int x, int y)
{
    assert(mapData.size() == width * height);
    assert(x >= 0 && y >= 0 && x < width && y < height);
    return mapData[y * width + x];
}

Block * structure::GetBlockSafe(int x, int y)
{
    assert(mapData.size() == width * height);
    if (x < 0 || y < 0 || x >= width || y >= height)
        return nullptr;

    return &mapData[y * width + x];
}

void structure::copyFromMap(GameMap &map, Vector2 start, Vector2 end)
{
    if (end.x > map.width)
        end.x = map.width - 1;
    if (end.y > map.height)
        end.y = map.height - 1;
    if (start.x > map.width)
        start.x = map.width - 1;
    if (start.y > map.height)
        start.y = map.height - 1;

    if (end.x < 0)
        end.x = 0;
    if (end.y < 0)
        end.y = 0;
    if (start.x < 0)
        start.x = 0;
    if (start.y < 0)
        start.y = 0;

    Vector2 size = Vector2(end.x - start.x + 1, end.y - start.y + 1);
    if (size.x > map.width || size.y > map.height)
        return;

    Create(size.x, size.y);

    for (int y = 0; y < size.y; ++y)
        for (int x = 0; x < size.x; ++x)
            GetBlockUnsafe(x, y) = map.GetBlockUnsafe(x + start.x, y + start.y);
}

void structure::pasteIntoMap(GameMap &map, const Vector2 &start)
{
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            if (Block* b = map.GetBlockSafe(x + start.x, y + start.y))
                *b = GetBlockUnsafe(x, y);
}
