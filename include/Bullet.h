#ifndef BULLET_H
#define BULLET_H

#include <SDL.h>

#include "DynamicObject.h"

class Bullet : public DynamicObject {
public:
    Bullet(b2World& world, float x, float y, float dirX, float dirY, bool doJogador, SDL_Renderer* renderer);

    void update(float dt) override;
    void draw(SDL_Renderer* renderer, int cameraX) override;

    bool isAlive() const { return viva; }
    void kill() { viva = false; }
    bool isFromPlayer() const { return doJogador; }

private:
    bool viva = true;
    bool doJogador;
    float tempoVida = 0.0f;
    SDL_Texture* textura = nullptr;

    static constexpr float VELOCIDADE = 50.0f;
    static constexpr float TEMPO_MAX_VIDA = 3.0f;
    static constexpr int TAMANHO = 48;
};

#endif
