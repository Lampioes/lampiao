#ifndef FENCE_H
#define FENCE_H

#include <SDL.h>
#include <box2d/box2d.h>

class Fence {
public:
    Fence(b2World& world, float x, float y, float width, float height);
    ~Fence() = default;

    void draw(SDL_Renderer* renderer, int cameraX);

    bool isDestroyed() const { return destroyed; }
    void takeDamage(int dmg);
    b2Body* getBody() const { return body; }
    float getX() const { return posX; }
    float getY() const { return posY; }
    float getWidth() const { return w; }

    void destroyBody(b2World& world);

private:
    b2Body* body = nullptr;
    float posX, posY, w, h;
    int health = 5;
    bool destroyed = false;

    static constexpr float P2M = 30.0f;
};

#endif