#ifndef FENCE_H
#define FENCE_H

#include <SDL.h>

#include "StaticObject.h"

class Fence : public StaticObject {
public:
    Fence(b2World& world, float x, float y, float largura, float altura);

    void draw(SDL_Renderer* renderer, int cameraX) override;

    bool isDestroyed() const { return destruida; }
    void takeDamage(int dano);
    float getX() const { return posX; }
    float getY() const { return posY; }
    float getWidth() const { return largura; }

private:
    float largura, altura;
    int vida = 5;
    bool destruida = false;
};

#endif
