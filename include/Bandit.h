#ifndef BANDIT_H
#define BANDIT_H

#include <SDL.h>
#include <SDL_image.h>
#include <box2d/box2d.h>

class Bandit {
public:
    Bandit(b2World& world, SDL_Renderer* renderer, float x, float y, int id);
    ~Bandit() = default;

    void update(float dt);
    void draw(SDL_Renderer* renderer, int cameraX);

    bool isAlive() const { return alive; }
    void takeDamage();
    b2Body* getBody() const { return body; }
    int getId() const { return banditId; }

    bool shouldShoot(float dt);
    float getShootDirX(float playerX);

    void destroyBody(b2World& world);
    bool shouldShoot(float dt, float playerx);

private:
    b2Body* body = nullptr;
    int banditId;
    bool alive = true;
    float shootTimer = 0.0f;
    float shootCooldown = 2.5f;
    int health = 3;
    SDL_Texture* texture = nullptr;
    bool facingLeft = false;
    bool reloading = false;
    float reloadTimer = 0.0f;
    static constexpr float RELOAD_TIME = 1.5f;
    static constexpr float SHOOT_RANGE = 400.0f;

    static constexpr float P2M = 30.0f;
    static constexpr int WIDTH = 150;
    static constexpr int HEIGHT = 180;
};

#endif