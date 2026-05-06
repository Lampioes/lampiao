#ifndef FENCE_H
#define FENCE_H

#include <SDL.h>
#include <box2d/box2d.h>

class Fence {
public:
    Fence(b2World& world, float x, float y, float largura, float altura);
    ~Fence() = default;

    void draw(SDL_Renderer* renderer, int cameraX);

    bool isDestroyed() const { return destruida; }
    void takeDamage(int dano);
    b2Body* getBody() const { return corpo; }
    float getX() const { return posX; }
    float getY() const { return posY; }
    float getWidth() const { return largura; }

    void destroyBody(b2World& world);

private:
    b2Body* corpo = nullptr;
    float posX, posY, largura, altura;
    int vida = 5;
    bool destruida = false;

    static constexpr float P2M = 30.0f;
};

#endif
