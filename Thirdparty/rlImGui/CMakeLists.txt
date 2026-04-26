cmake_minimum_required(VERSION 4.1)
project(rlImgui)

find_package(raylib CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_library(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE "rlImGui.cpp" "rlImGui.h")

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/")

target_link_libraries(${PROJECT_NAME} PUBLIC imgui::imgui raylib)
