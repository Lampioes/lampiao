#ifndef BANDIT_H
#define BANDIT_H

#include <SDL2/SDL.h>
#include <box2d/box2d.h>

class Bandit {
public:
    Bandit(b2World& world, float x, float y, int id);
    ~Bandit() = default;

    void update(float dt);
    void draw(SDL_Renderer* renderer, int cameraX);

    bool isAlive() const { return alive; }
    void takeDamage();
    b2Body* getBody() const { return body; }
    int getId() const { return banditId; }

    bool shouldShoot(float dt);
    float getShootDirX(float playerX) const;

    void destroyBody(b2World& world);

private:
    b2Body* body = nullptr;
    int banditId;
    bool alive = true;
    float shootTimer = 0.0f;
    float shootCooldown = 2.5f;
    int health = 3;

    static constexpr float P2M = 30.0f;
    static constexpr int WIDTH = 40;
    static constexpr int HEIGHT = 70;
};

#endif