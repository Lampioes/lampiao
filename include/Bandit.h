#ifndef BANDIT_H
#define BANDIT_H

#include <SDL.h>

#include "DynamicObject.h"
#include "Sprite.h"

class Bandit : public DynamicObject {
public:
    Bandit(b2World& world, SDL_Renderer* renderer, const b2Vec2& p, int id);

    void update(float dt) override;
    void draw(SDL_Renderer* renderer, int cameraX) override;

    bool isAlive() const { return vivo; }
    void takeDamage();
    int getId() const { return idBandido; }

    float getShootDirX(float playerX);

    bool shouldShoot(float dt, float playerx);

private:
    int idBandido;
    bool vivo = true;
    float temporizadorTiro = 0.0f;
    float recargaTiro = 2.5f;
    int vida = 3;
    Sprite sprite;
    bool viradoEsquerda = false;
    bool recarregando = false;
    float temporizadorRecarga = 0.0f;

    static constexpr float TEMPO_RECARGA = 1.5f;
    static constexpr float ALCANCE_TIRO = 400.0f;
    static constexpr int LARGURA = 150;
    static constexpr int ALTURA = 180;
};

#endif
