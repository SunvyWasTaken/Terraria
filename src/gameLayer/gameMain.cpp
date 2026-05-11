//
// Created by sunvy on 19/04/2026.
//

#include <format>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <raylib.h>
#include <raymath.h>

#include "gameMain.h"

#include "assetManager.h"
#include "gameMap.h"
#include "helpers.h"
#include "random.h"
#include "rlImGui.h"
#include "saveMap.h"
#include "structure.h"
#include "WorldGen/worldGenerator.h"

namespace
{
    float speed = 7.f;

    std::ranlux24_base rng{std::random_device{}()};

    struct GameData
    {
        GameMap gameMap;
        Camera2D camera;
        Block currentSelectBlock;
        Random noise;
        uint32_t Seed = 69;

        Vector2 selectionStart = {};
        Vector2 selectionEnd = {};
        structure copyStructure;

        std::string saveName{};
    }gameData;

    AssetManager assetManager;

    void CloseLevel()
    {
        gameData = {};
    }

    void LoadLevel()
    {
        gameData = {};
    }

    bool IsImGuiHovered = false;
}

bool InitGame()
{
    assetManager.LoadAll();

    gameData.Seed = GetRandomInt(rng, 0, std::numeric_limits<int>::max());

    GenerateWorld(gameData.gameMap, gameData.Seed);

    gameData.camera.target = {20, 120};
    gameData.camera.rotation = 0.f;
    gameData.camera.zoom = 10.f;

    return true;
}

bool UpdateGame()
{
    float dt = GetFrameTime();
    if (dt > 1.f / 5.f)
        dt = 1.f / 5.f;

    gameData.camera.offset = {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f};

    if (IsKeyDown(KEY_A))
        gameData.camera.target.x -= speed * dt;
    if (IsKeyDown(KEY_D))
        gameData.camera.target.x += speed * dt;
    if (IsKeyDown(KEY_W))
        gameData.camera.target.y -= speed * dt;
    if (IsKeyDown(KEY_S))
        gameData.camera.target.y += speed * dt;

    Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);
    int blockX = (int)floor(worldPos.x);
    int blockY = (int)floor(worldPos.y);

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !IsImGuiHovered)
    {
        Block* block = gameData.gameMap.GetBlockSafe(blockX, blockY);
        if (block != nullptr)
            *block = {};
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && !IsImGuiHovered)
        if (Block* block = gameData.gameMap.GetBlockSafe(blockX, blockY))
            block->type = gameData.currentSelectBlock.type;

    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) && !IsImGuiHovered)
    {
        if (Block* block = gameData.gameMap.GetBlockSafe(blockX, blockY))
        {
            gameData.currentSelectBlock.type = block->type;
        }
    }

    gameData.camera.zoom += GetMouseWheelMove();

    if (IsMouseButtonDown(MOUSE_BUTTON_EXTRA))
        gameData.selectionStart = Vector2{(float)blockX, (float)blockY};
    if (IsMouseButtonDown(MOUSE_BUTTON_SIDE))
        gameData.selectionEnd = Vector2{(float)blockX, (float)blockY};

    if (gameData.selectionStart.x > gameData.selectionEnd.x)
        std::swap(gameData.selectionStart.x, gameData.selectionEnd.x);
    if (gameData.selectionStart.y > gameData.selectionEnd.y)
        std::swap(gameData.selectionStart.y, gameData.selectionEnd.y);

    if (IsKeyDown(KEY_V))
        gameData.copyStructure.pasteIntoMap(gameData.gameMap, Vector2{(float)blockX, (float)blockY});

    // Start rendering
    BeginMode2D(gameData.camera);

    Vector2 topLeftView = GetScreenToWorld2D({0, 0}, gameData.camera);
    Vector2 bottomLeftView = GetScreenToWorld2D({(float)GetScreenWidth(), (float)GetScreenHeight()}, gameData.camera);

    int startXView = (int)floor(topLeftView.x - 1);
    int startYView = (int)floor(topLeftView.y - 1);
    int endXView = (int)floor(bottomLeftView.x + 1);
    int endYView = (int)floor(bottomLeftView.y + 1);

    startXView = Clamp(startXView, 0, gameData.gameMap.width - 1);
    endXView = Clamp(endXView, 0, gameData.gameMap.width - 1);

    startYView = Clamp(startYView, 0, gameData.gameMap.height - 1);
    endYView = Clamp(endYView, 0, gameData.gameMap.height - 1);

    for (int y = startYView; y < endYView; ++y)
        for (int x = startXView; x < endXView; ++x)
        {
            std::ranlux24_base r{Hash(x, y, gameData.Seed)};
            int indexVar = GetRandomInt(r, 0, 3);

            auto& bb = gameData.gameMap.GetBackBlockUnsafe(x, y);
            DrawTexturePro(
                assetManager.Wall,
                getTextureAtlas(bb.type, indexVar, 32, 32),
                {(float)x, (float)y, 1, 1},
                {},
                0,
                WHITE);


            auto &b = gameData.gameMap.GetBlockUnsafe(x, y);
            if (b.type == Block::air)
                continue;

            if (b.type == Block::woodLog)
            {
                std::uint8_t mask = 0;

                if (Block* bl = gameData.gameMap.GetBlockSafe(x - 1, y))
                    if (bl->type == Block::leaves)
                        mask |= 1 << 0; // droite

                if (Block* bl = gameData.gameMap.GetBlockSafe(x + 1, y))
                    if (bl->type == Block::leaves)
                        mask |= 1 << 1; // gauche

                if (Block* bl = gameData.gameMap.GetBlockSafe(x, y - 1))
                    if (bl->type == Block::leaves)
                        mask |= 1 << 2; // haut

                if (Block* bl = gameData.gameMap.GetBlockSafe(x, y - 1))
                    if (bl->type == Block::woodLog)
                        mask |= 1 << 3; // stump

                if (Block* bl = gameData.gameMap.GetBlockSafe(x, y + 1))
                    if (bl->type == Block::woodLog)
                        mask |= 1 << 4; // roots

                Rectangle targetRec = getTextureAtlas(0, 0, 32, 32);

                if ((mask & (1 << 0 | 1 << 1 | 1 << 2)) == (1 << 0 | 1 << 1 | 1 << 2))
                    targetRec = getTextureAtlas(5, 0, 32, 32);
                else if ((mask & (1 << 0 | 1 << 1)) == (1 << 0 | 1 << 1))
                    targetRec = getTextureAtlas(1, indexVar, 32, 32);
                else if ((mask & 1 << 0) == 1 << 0)
                    targetRec = getTextureAtlas(3, indexVar, 32, 32);
                else if ((mask & 1 << 1) == 1 << 1)
                    targetRec = getTextureAtlas(2, indexVar, 32, 32);
                else if (!(mask & 1 << 3) && !(mask & 1 << 4))
                    targetRec = getTextureAtlas(7, indexVar, 32, 32);
                else if (!(mask & 1 << 4))
                    targetRec = getTextureAtlas(4, indexVar, 32, 32);
                else if (!(mask & 1 << 3))
                    targetRec = getTextureAtlas(6, indexVar, 32, 32);

                DrawTexturePro(
                    assetManager.Tree,
                    targetRec,
                    {(float)x, (float)y, 1, 1},
                    {},
                    0,
                    WHITE);
            }
            else if (b.type == Block::leaves)
            {
                float noise = abs(gameData.noise.PerlinNoise(x, y));
                unsigned char alpha = floor(Lerp(255, 127, noise));

                Color leavesColor = {255, alpha, 255, 255};

                DrawTexturePro(
                  assetManager.textures,
                  getTextureAtlas(b.type, indexVar, 32, 32),
                  {(float)x, (float)y, 1, 1},
                  {},
                  0,
                  leavesColor);
            }
            else
            {
                DrawTexturePro(
                  assetManager.textures,
                  getTextureAtlas(b.type, indexVar, 32, 32),
                  {(float)x, (float)y, 1, 1},
                  {},
                  0,
                  WHITE);
            }
        }

    if (gameData.currentSelectBlock.type != Block::air && gameData.currentSelectBlock.type < Block::BLOCKS_COUNT)
    {
        DrawTexturePro(
            assetManager.textures,
            getTextureAtlas(gameData.currentSelectBlock.type, 0, 32, 32),
            {(float)blockX, (float)blockY, 1, 1},
            {},
            0.f,
            {255, 255, 255, 127});
    }
    else
        DrawTexturePro(
            assetManager.Frame,
            {0,0, (float)assetManager.Frame.width, (float)assetManager.Frame.height},
            {(float)blockX, (float)blockY, 1, 1},
            {},
            0.f,
            WHITE);

    Rectangle rect;
    rect.x = gameData.selectionStart.x;
    rect.y = gameData.selectionStart.y;
    rect.width = gameData.selectionEnd.x - rect.x;
    rect.height = gameData.selectionEnd.y - rect.y;

    rect.width++;
    rect.height++;

    DrawRectangleLinesEx(rect, 0.1, {20, 101, 250, 145});

    EndMode2D();

    rlImGuiBegin();

    ImGui::Begin("Tools");
    IsImGuiHovered = ImGui::IsWindowHovered() || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemFocused();
    ImGui::DragFloat("Noise amplitude", &gameData.noise.amplitude, 0.001f, 0.001f);
    ImGui::SliderFloat("Zoom##1", &gameData.camera.zoom, 1.f, 100.f);
    speed = Lerp(50, 7, gameData.camera.zoom / 100.f);

    ImGui::Separator();

    if (ImGui::Button("Copy"))
    {
        gameData.copyStructure.copyFromMap(gameData.gameMap, gameData.selectionStart, gameData.selectionEnd);
    }

    ImGui::InputText("File Name", &gameData.saveName);

    if (ImGui::Button("Save to file"))
    {
        std::string path = RESOURCES_PATH "structures/";
        path += gameData.saveName;
        path += ".bin";

        saveBlockDatatoFile(gameData.copyStructure.mapData, gameData.copyStructure.width, gameData.copyStructure.height, path);
    }

    if (ImGui::Button("Load from file"))
    {
        std::string path = RESOURCES_PATH "structures/";
        path += gameData.saveName;
        path += ".bin";

        loadBlockDatatoFile(gameData.copyStructure.mapData, gameData.copyStructure.width, gameData.copyStructure.height, path);
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Blocks"))
        if (ImGui::BeginTable("Block Selector", 5))
        {
            for (int row = 0; row < 11; ++row)
            {
                ImGui::TableNextRow();
                for (int col = 0; col < 5; ++col)
                {
                    ImGui::TableSetColumnIndex(col);
                    int index = col + row * 5;
                    Rectangle atlas = getTextureAtlas(index, 0, 32, 32);
                    ImVec2 startUv = {atlas.x / assetManager.textures.width, atlas.y / assetManager.textures.height};
                    ImVec2 endUv = {(atlas.x + 32) / assetManager.textures.width, (atlas.y - 32) / assetManager.textures.height};
                    if (ImGui::ImageButton(std::format("{}", Block::blockNames[index]).c_str(), assetManager.textures.id, {32, 32}, startUv, endUv))
                        gameData.currentSelectBlock.type = index;
                    ImGui::SetItemTooltip("%s", Block::blockNames[index].c_str());
                }
            }
            ImGui::EndTable();
        }

    ImGui::End();

    rlImGuiEnd();

    DrawFPS(10, 10);

    return true;
}

void CloseGame()
{
    CloseLevel();
}
