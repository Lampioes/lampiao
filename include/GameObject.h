#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <SDL.h>

class GameObject {
public:
    virtual ~GameObject() = default;

    virtual void draw(SDL_Renderer* renderer, int cameraX) = 0;

    float getPosX() const { return posX; }
    float getPosY() const { return posY; }

protected:
    GameObject(float x, float y) : posX(x), posY(y) {}

    float posX = 0.0f;
    float posY = 0.0f;

    static constexpr float P2M = 30.0f;
};

#endif
