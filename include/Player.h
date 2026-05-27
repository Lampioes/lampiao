#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <vector>
#include <algorithm>

#include "DynamicObject.h"

class Player : public DynamicObject {
public:
    Player(b2World& world, SDL_Renderer* renderer, float x, float y);

    void update(float dt = 1.0f / 60.0f) override;
    void draw(SDL_Renderer* renderer, int cameraX = 0) override;

    void moveRight();
    void moveLeft();
    void jump();

    bool canShoot() const { return recargaTiro <= 0.0f; }
    void resetShootCooldown() { recargaTiro = TEMPO_RECARGA_TIRO; }
    float getShootDirX() const { return viradoEsquerda ? -1.0f : 1.0f; }

    void takeDamage();
    int getHealth() const { return vida; }
    bool isAlive() const { return vida > 0; }
    bool isFacingLeft() const { return viradoEsquerda; }

    bool getIsMoving() const { return emMovimento; }

    void setOnGround(bool v) { noChao = v; }
    void captureCow();

private:
    std::vector<SDL_Texture*> texturas;
    SDL_Rect retanguloRender;
    int frameAtual = 1;
    bool viradoEsquerda = false;
    bool emMovimento = false;
    float alvoX = 0.0f;
    float velocidadeMovimento = 10.0f;
    float ultimoX = 0.0f;
    int framesPreso = 0;
    int vida = 5;
    float recargaTiro = 0.0f;
    bool noChao = true;

    static constexpr float DISTANCIA_MOVIMENTO = 10.0f;
    static constexpr float MIN_X = -10000.0f;
    static constexpr float MAX_X =  10000.0f;
    static constexpr float IMPULSO_PULO = -35.0f;
    static constexpr float TEMPO_RECARGA_TIRO = 0.4f;

    void clampPosition();
};

#endif
