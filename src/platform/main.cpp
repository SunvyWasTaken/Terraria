// Sunset Inc.

#include <iostream>

#include <raylib.h>
#include <imgui.h>

#include "gameMain.h"
#include "rlImGui.h"

int main()
{

#if NDEBUG
    SetTraceLogLevel(LOG_NONE);
#endif

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    InitWindow(1280, 720, "raylib window");

    rlImGuiSetup(true);

    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.FontGlobalScale = 1.f;

    ImGui::StyleColorsDark();

    if (!InitGame())
    {
        CloseWindow();
        return 0;
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        if (!UpdateGame())
            CloseWindow();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    CloseGame();

    return 0;
}