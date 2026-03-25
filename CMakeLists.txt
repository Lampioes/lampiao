cmake_minimum_required(VERSION 3.20)
project(JogoVelhoOeste)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Busca o SDL2
find_package(SDL2 REQUIRED)

# 2. Busca o Box2D
find_package(box2d REQUIRED)

find_package(SDL2_image REQUIRED)

# 3. Define onde está a sua My-lib (header-only)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../my-lib/include")

# 4. Cria o executável
add_executable(JogoVelhoOeste
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
)

# 5. Linka as bibliotecas ao seu projeto
# Isso diz ao compilador para "unir" o código do SDL e Box2D ao seu executável
target_link_libraries(JogoVelhoOeste PRIVATE SDL2::SDL2 box2d::box2d SDL2_image::SDL2_image)

# 6. Garante que os headers encontrados pelo find_package fiquem no include path
target_include_directories(JogoVelhoOeste PRIVATE ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})