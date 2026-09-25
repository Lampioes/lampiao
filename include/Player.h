#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <algorithm>

#include "Animation.h"
#include "DynamicObject.h"
#include "SpriteCatalog.h"

class Player : public DynamicObject {
public:
    Player(b2World& world, const SpriteCatalog& sprites, const b2Vec2& p);

    // o jogador cuida do proprio teclado; quem entrega o evento e a cena
    void handleEvent(const SDL_Event& evento) override;
    void update(float dt = 1.0f / 60.0f) override;
    void draw(SDL_Renderer* renderer, const Camera& camera) override;

    void moveRight();
    void moveLeft();
    void stop();
    void jump();

    bool canShoot() const { return recargaTiro <= 0.0f; }
    void resetShootCooldown() { recargaTiro = TEMPO_RECARGA_TIRO; }
    float getShootDirX() const { return viradoEsquerda ? -1.0f : 1.0f; }

    // o mundo pergunta se o jogador pulou/atirou nesse frame pra tocar o som e criar a bala
    bool consumeJumped();
    bool consumeShot();

    void takeDamage();
    int getHealth() const { return vida; }
    bool isAlive() const override { return vida > 0; }
    bool isFacingLeft() const { return viradoEsquerda; }

    bool getIsMoving() const { return emMovimento; }

    void setOnGround(bool v) { noChao = v; }
    bool isOnGround() const { return noChao; }
    void captureCow();
    bool hasWon() const { return vacasCapturadas >= vacasCapturadasPraGanharOJogo; }
    int getCapturedCows() const { return vacasCapturadas; }
    int getCowsToWin() const { return vacasCapturadasPraGanharOJogo; }

private:
    static constexpr float DISTANCIA_MOVIMENTO = 10.0f;
    static constexpr float MIN_X = -10000.0f;
    static constexpr float MAX_X =  10000.0f;
    static constexpr float IMPULSO_PULO = -35.0f;
    static constexpr float TEMPO_RECARGA_TIRO = 0.4f;
    static constexpr int FRAME_PARADO = 1;
    static constexpr int LARGURA = 150;
    static constexpr int ALTURA = 150;

    Animation animacaoCorrer;
    SDL_Rect retanguloRender{0, 0, LARGURA, ALTURA};
    bool viradoEsquerda = false;
    bool emMovimento = false;
    float alvoX = 0.0f;
    float velocidadeMovimento = 10.0f;
    float ultimoX = 0.0f;
    int framesPreso = 0;
    int vida = 5;
    float recargaTiro = 0.0f;
    bool noChao = true;
    bool pulouAgora = false;
    bool atirouAgora = false;
    int vacasCapturadas = 0;
    int vacasCapturadasPraGanharOJogo = 10;

    void clampPosition();
};

#endif
