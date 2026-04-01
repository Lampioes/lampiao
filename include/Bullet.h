#ifndef BULLET_H
#define BULLET_H

#include <SDL.h>
#include <box2d/box2d.h>

class Bullet {
public:
    Bullet(b2World& world, float x, float y, float dirX, float dirY, bool fromPlayer, SDL_Renderer* renderer);
    ~Bullet() = default;

    void draw(SDL_Renderer* renderer, int cameraX);
    bool isAlive() const { return alive; }
    void kill() { alive = false; }
    bool isFromPlayer() const { return fromPlayer; }
    b2Body* getBody() const { return body; }

    void destroyBody(b2World& world);

private:
    b2Body* body = nullptr;
    bool alive = true;
    bool fromPlayer;
    float lifetime = 0.0f;
    SDL_Texture* texture = nullptr;

    static constexpr float P2M = 30.0f;
    static constexpr float SPEED = 15.0f;
    static constexpr float MAX_LIFETIME = 3.0f;
    static constexpr int SIZE = 48;

public:
    void update(float dt);
};

#endif