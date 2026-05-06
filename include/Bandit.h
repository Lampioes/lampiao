#ifndef BANDIT_H
#define BANDIT_H

#include <SDL.h>
#include <SDL_image.h>
#include <box2d/box2d.h>

class Bandit {
public:
    Bandit(b2World& world, SDL_Renderer* renderer, float x, float y, int id);
    ~Bandit() = default;

    void update(float dt);
    void draw(SDL_Renderer* renderer, int cameraX);

    bool isAlive() const { return vivo; }
    void takeDamage();
    b2Body* getBody() const { return corpo; }
    int getId() const { return idBandido; }

    float getShootDirX(float playerX);

    void destroyBody(b2World& world);
    bool shouldShoot(float dt, float playerx);

private:
    b2Body* corpo = nullptr;
    int idBandido;
    bool vivo = true;
    float temporizadorTiro = 0.0f;
    float recargaTiro = 2.5f;
    int vida = 3;
    SDL_Texture* textura = nullptr;
    bool viradoEsquerda = false;
    bool recarregando = false;
    float temporizadorRecarga = 0.0f;
    static constexpr float TEMPO_RECARGA = 1.5f;
    static constexpr float ALCANCE_TIRO = 400.0f;

    static constexpr float P2M = 30.0f;
    static constexpr int LARGURA = 150;
    static constexpr int ALTURA = 180;
};

#endif
