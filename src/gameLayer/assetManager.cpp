//
// Created by sunvy on 20/04/2026.
//

#include "assetManager.h"

void AssetManager::LoadAll()
{
    dirt = LoadTexture(RESOURCES_PATH "dirt.png");

    textures = LoadTexture(RESOURCES_PATH "textures.png");

    Frame = LoadTexture(RESOURCES_PATH "frame.png");

    Tree = LoadTexture(RESOURCES_PATH "treetextures.png");

    Wall = LoadTexture(RESOURCES_PATH "texturesWithBackgroundVersion.png");
}
