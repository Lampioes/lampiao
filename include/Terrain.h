#ifndef TERRAIN_H
#define TERRAIN_H

#include <SDL.h>
#include <box2d/box2d.h>
#include <array>
#include <vector>

class Terrain {
public:
    static constexpr int NUM_ZONAS = 5;
    static constexpr int PONTOS_POR_ZONA = 50;
    static constexpr int TOTAL_PONTOS = NUM_ZONAS * PONTOS_POR_ZONA;
    static constexpr int NUM_REPETICOES = 50;
    static constexpr float P2M = 30.0f;
    static constexpr int TELA_W = 1920;
    static constexpr int TELA_H = 1080;

    Terrain(b2World& world);
    ~Terrain();

    void draw(SDL_Renderer* renderer, int cameraX);

    float getHeightAt(float pixelX) const;

    b2Body* getBody() const { return corpoChao; }

private:
    b2Body* corpoChao = nullptr;
    std::array<float, TOTAL_PONTOS + 1> alturas{};
    float espacamentoPontos;

    void generateHeightmap();
    void createPhysicsBody(b2World& world);
};

#endif
