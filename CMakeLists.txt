cmake_minimum_required(VERSION 3.20)
project(JogoVelhoOeste)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
find_package(box2d REQUIRED)
find_package(SDL2_image REQUIRED)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../my-lib/include")

add_executable(JogoVelhoOeste
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Terrain.cpp
    src/Bullet.cpp
    src/Bandit.cpp
    src/Cow.cpp
    src/Fence.cpp
    src/Rope.cpp
    src/ContactListener.cpp
)

target_link_libraries(JogoVelhoOeste PRIVATE SDL2::SDL2 box2d::box2d SDL2_image::SDL2_image)