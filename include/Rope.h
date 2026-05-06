#ifndef ROPE_H
#define ROPE_H

#include <SDL.h>
#include <box2d/box2d.h>
#include <list>

struct RopeSegment {
    b2Body* corpo = nullptr;
    b2Joint* juncao = nullptr;
};

class Rope {
public:
    Rope() = default;
    ~Rope() = default;

    void launch(b2World& world, float startX, float startY, float dirX, float dirY);
    void retract(b2World& world);
    void update(float dt);
    void draw(SDL_Renderer* renderer, int cameraX);
    bool isActive() const { return ativa; }

    b2Body* getTipBody() const;

    void attachTo(b2World& world, b2Body* alvo);
    bool isAttached() const { return presa; }

private:
    std::list<RopeSegment> segmentos;
    bool ativa = false;
    bool presa = false;
    float tempoVida = 0.0f;

    static constexpr float P2M = 30.0f;
    static constexpr int NUM_SEGMENTOS = 8;
    static constexpr float COMPRIMENTO_SEGMENTO = 0.5f;
    static constexpr float TEMPO_MAX_VIDA = 2.0f;
    static constexpr float VELOCIDADE_LANCAMENTO = 10.0f;
};

#endif
