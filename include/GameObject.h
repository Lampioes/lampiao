#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <SDL.h>
#include <box2d/box2d.h>

class GameObject {
public:
    virtual ~GameObject() = default;

    virtual void update(float /*dt*/) {}
    virtual void draw(SDL_Renderer* renderer, int cameraX) = 0;
    virtual bool isAlive() const { return true; }
    virtual void destroyBody(b2World& /*world*/) {}

    const b2Vec2& getPosition() const { return posicao; }
    float getPosX() const { return posicao.x; }
    float getPosY() const { return posicao.y; }

protected:
    explicit GameObject(const b2Vec2& p) : posicao(p) {}

    b2Vec2 posicao{0.0f, 0.0f};

    static constexpr float P2M = 30.0f;
};

#endif
