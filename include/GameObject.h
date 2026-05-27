#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <SDL.h>
#include <box2d/box2d.h>

class GameObject {
public:
    virtual ~GameObject() = default;

    virtual void draw(SDL_Renderer* renderer, int cameraX) = 0;

    const b2Vec2& getPosition() const { return posicao; }
    float getPosX() const { return posicao.x; }
    float getPosY() const { return posicao.y; }

protected:
    GameObject(float x, float y) : posicao(x, y) {}
    GameObject(const b2Vec2& p) : posicao(p) {}

    b2Vec2 posicao{0.0f, 0.0f};

    static constexpr float P2M = 30.0f;
};

#endif
