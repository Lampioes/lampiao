#ifndef BULLET_H
#define BULLET_H

#include <SDL.h>
#include <box2d/box2d.h>

class Bullet {
public:
    Bullet(b2World& world, float x, float y, float dirX, float dirY, bool doJogador, SDL_Renderer* renderer);
    ~Bullet() = default;

    void draw(SDL_Renderer* renderer, int cameraX);
    bool isAlive() const { return viva; }
    void kill() { viva = false; }
    bool isFromPlayer() const { return doJogador; }
    b2Body* getBody() const { return corpo; }

    void destroyBody(b2World& world);

private:
    b2Body* corpo = nullptr;
    bool viva = true;
    bool doJogador;
    float tempoVida = 0.0f;
    SDL_Texture* textura = nullptr;

    static constexpr float P2M = 30.0f;
    static constexpr float VELOCIDADE = 15.0f;
    static constexpr float TEMPO_MAX_VIDA = 3.0f;
    static constexpr int TAMANHO = 48;

public:
    void update(float dt);
};

#endif
