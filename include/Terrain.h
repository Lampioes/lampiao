#ifndef TERRAIN_H
#define TERRAIN_H

#include <SDL.h>
#include <array>
#include <vector>

#include "StaticObject.h"

class Terrain : public StaticObject {
public:
    static constexpr int NUM_ZONAS = 5;
    static constexpr int PONTOS_POR_ZONA = 50;
    static constexpr int TOTAL_PONTOS = NUM_ZONAS * PONTOS_POR_ZONA;
    static constexpr int NUM_REPETICOES = 50;
    static constexpr int TELA_W = 1920;
    static constexpr int TELA_H = 1080;

    Terrain(b2World& world);

    void draw(SDL_Renderer* renderer, int cameraX) override;

    float getHeightAt(float pixelX) const;

private:
    std::array<float, TOTAL_PONTOS + 1> alturas{};
    float espacamentoPontos;

    void generateHeightmap();
    void createChainFixture();
};

#endif
