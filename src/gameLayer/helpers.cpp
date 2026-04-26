//
// Created by sunvy on 20/04/2026.
//

#include "helpers.h"

Rectangle getTextureAtlas(int x, int y, int cellSizePixelsX, int cellSizePixelsY)
{
    return Rectangle{(float)x * cellSizePixelsX, (float)y * cellSizePixelsY, (float)cellSizePixelsX, (float)cellSizePixelsY};
}
