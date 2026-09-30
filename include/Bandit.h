#ifndef BANDIT_H
#define BANDIT_H

#include <SDL.h>

#include "DynamicObject.h"
#include "Sprite.h"

class Bandit : public DynamicObject {
public:
    Bandit(b2World& world, const Sprite& sprite, const b2Vec2& p, int id);

    void update(float dt) override;
    void draw(SDL_Renderer* renderer, const Camera& camera) override;

    bool isAlive() const override { return vivo; }
    void takeDamage();
    int getId() const { return idBandido; }

    float getShootDirX(float playerX);

    bool shouldShoot(float dt, float playerx);

private:
    static constexpr float TEMPO_RECARGA = 1.5f;
    static constexpr float ALCANCE_TIRO = 400.0f;
    static constexpr int LARGURA = 150;
    static constexpr int ALTURA = 180;

    int idBandido;
    bool vivo = true;
    float temporizadorTiro = 0.0f;
    float recargaTiro = 2.5f;
    int vida = 3;
    const Sprite* sprite;
    bool viradoEsquerda = false;
    bool recarregando = false;
    float temporizadorRecarga = 0.0f;
    SDL_Rect retanguloRender{0, 0, LARGURA, ALTURA};
};

#endif
