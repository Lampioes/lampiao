cmake_minimum_required(VERSION 3.20)
project(JogoVelhoOeste LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(box2d REQUIRED)

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

target_link_libraries(JogoVelhoOeste
    PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
        box2d::box2d
)

if (TARGET SDL2::SDL2main)
    target_link_libraries(JogoVelhoOeste PRIVATE SDL2::SDL2main)
endif()
