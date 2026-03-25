#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <box2d/box2d.h>
#include <vector>
#include <algorithm>

class Player {
private:
    b2Body* body;
    std::vector<SDL_Texture*> textures;
    SDL_Rect renderRect;
    int currentFrame = 1;
    bool facingLeft = false;
    bool isMoving = false;
    float targetX = 0.0f;
    float moveSpeed = 0.25f;
    float lastX = 0.0f;
    int stuckFrames = 0;
    int health = 5;
    float shootCooldown = 0.0f;

    static constexpr float P2M = 30.0f;
    static constexpr float MOVE_DISTANCE = 10.0f;
    static constexpr float MIN_X = -10000.0f;
    static constexpr float MAX_X =  10000.0f;
    static constexpr float JUMP_IMPULSE = -5.0f;
    static constexpr float SHOOT_COOLDOWN_TIME = 0.4f;

    void clampPosition();

public:
    Player(b2World& world, SDL_Renderer* renderer, float x, float y);
    void update(float dt = 1.0f / 60.0f);
    void draw(SDL_Renderer* renderer, int cameraX = 0);
    void moveRight();
    void moveLeft();
    void jump();

    bool canShoot() const { return shootCooldown <= 0.0f; }
    void resetShootCooldown() { shootCooldown = SHOOT_COOLDOWN_TIME; }
    float getShootDirX() const { return facingLeft ? -1.0f : 1.0f; }

    void takeDamage();
    int getHealth() const { return health; }
    bool isAlive() const { return health > 0; }
    bool isFacingLeft() const { return facingLeft; }

    bool getIsMoving() const { return isMoving; }
    b2Body* getBody() const { return body; }
};

#endif