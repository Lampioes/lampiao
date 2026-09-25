#ifndef BULLET_H
#define BULLET_H

#include <SDL.h>

#include "DynamicObject.h"
#include "Sprite.h"

class Bullet : public DynamicObject {
public:
    Bullet(b2World& world, const Sprite& sprite, const b2Vec2& p, const b2Vec2& dir, bool doJogador);

    void update(float dt) override;
    void draw(SDL_Renderer* renderer, const Camera& camera) override;

    bool isAlive() const override { return viva; }
    void kill() { viva = false; }
    bool isFromPlayer() const { return doJogador; }

private:
    static constexpr float VELOCIDADE = 50.0f;
    static constexpr float TEMPO_MAX_VIDA = 3.0f;
    static constexpr int TAMANHO = 48;

    bool viva = true;
    bool doJogador;
    float tempoVida = 0.0f;
    const Sprite* sprite;
    SDL_Rect retanguloRender{0, 0, TAMANHO, TAMANHO};
};

#endif
