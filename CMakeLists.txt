cmake_minimum_required(VERSION 3.20)
project(JogoVelhoOeste)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
find_package(box2d REQUIRED)
# 3. Busca bibliotecas extras do SDL
# SDL2_image: carregamento de imagens
# SDL2_mixer: efeitos sonoros e musica de fundo
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

# 4. Define onde está a sua My-lib (header-only)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../my-lib/include")

# 5. Cria o executável
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

# 6. Garante que os headers encontrados pelo find_package fiquem no include path
target_include_directories(JogoVelhoOeste PRIVATE ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
)

# 6. Linka as bibliotecas ao seu projeto
# Isso diz ao compilador para "unir" o codigo do SDL, Box2D,
# carregamento de imagens e sistema de audio ao executavel
target_link_libraries(JogoVelhoOeste PRIVATE SDL2::SDL2 box2d::box2d SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer)