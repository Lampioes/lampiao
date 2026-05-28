cmake_minimum_required(VERSION 3.20)
project(JogoVelhoOeste LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

find_package(box2d 2 CONFIG QUIET)
if (box2d_FOUND)
    set(BOX2D_TARGET box2d::box2d)
else()
    include(FetchContent)

    set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
    set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "" FORCE)
    set(BOX2D_BUILD_DOCS OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        box2d
        URL https://github.com/erincatto/box2d/archive/refs/tags/v2.4.1.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(box2d)

    if (TARGET box2d::box2d)
        set(BOX2D_TARGET box2d::box2d)
    elseif (TARGET box2d)
        set(BOX2D_TARGET box2d)
    else()
        message(FATAL_ERROR "Box2D 2.x target not found")
    endif()
endif()

add_executable(JogoVelhoOeste
    src/main.cpp
    src/Game.cpp
    src/PhysicsObject.cpp
    src/StaticObject.cpp
    src/DynamicObject.cpp
    src/Player.cpp
    src/Terrain.cpp
    src/Bullet.cpp
    src/Bandit.cpp
    src/Cow.cpp
    src/Fence.cpp
    src/ContactListener.cpp
)

target_include_directories(JogoVelhoOeste
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../my-lib/include
)

if (TARGET SDL2::SDL2main)
    target_link_libraries(JogoVelhoOeste PRIVATE SDL2::SDL2main)
endif()

target_link_libraries(JogoVelhoOeste
    PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
        ${BOX2D_TARGET}
)
