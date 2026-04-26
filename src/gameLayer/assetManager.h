//
// Created by sunvy on 20/04/2026.
//

#ifndef TERRARIA_ASSETMANAGER_H
#define TERRARIA_ASSETMANAGER_H

#include <raylib.h>


struct AssetManager
{
    Texture2D dirt = {};
    Texture2D textures = {};
    Texture2D Frame = {};
    Texture2D Tree = {};
    Texture2D Wall = {};

    void LoadAll();
};


#endif //TERRARIA_ASSETMANAGER_H