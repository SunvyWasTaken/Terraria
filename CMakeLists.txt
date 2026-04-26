cmake_minimum_required(VERSION 4.1)

#! ! ! ! ! ! !
#set this to ON instead of OFF to ship the game!
#basically this will change the RESOURCES_PATH to be the local path
#! ! ! ! ! ! !
#delete the out folder after changing if visual studio doesn't recognize the change
set(PRODUCTION_BUILD OFF CACHE BOOL "Make this a production build" FORCE)

project(Terraria)

find_package(raylib CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_subdirectory(Thirdparty/rlImGui)
add_subdirectory(Thirdparty/FastNoiseSIMD)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

set(CMAKE_CXX_STANDARD 20)

add_executable("${PROJECT_NAME}")

if(PRODUCTION_BUILD)
    # setup the ASSETS_PATH macro to be in the root folder of your exe
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="./resources/")
    # remove the option to debug asserts.
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=1)
else()
    # This is useful to get an ASSETS_PATH in your IDE during development
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=0)
endif()

target_sources("${PROJECT_NAME}" PRIVATE ${MY_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE raylib glfw rlImgui fastNoiseSIMD)

target_include_directories("${PROJECT_NAME}" PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gameLayer"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/platform"
)
