#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
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

    static constexpr float P2M = 30.0f;
    static constexpr float MOVE_DISTANCE = 10.0f;
    static constexpr float MIN_X = -10000.0f;
    static constexpr float MAX_X =  10000.0f;
    static constexpr float JUMP_IMPULSE = -5.0f;

    void clampPosition();

public:
    Player(b2World& world, SDL_Renderer* renderer, float x, float y);
    void update();
    void draw(SDL_Renderer* renderer, int cameraX = 0);
    void moveRight();
    void moveLeft();
    void jump();
    bool getIsMoving() const { return isMoving; }
    b2Body* getBody() const { return body; }
};

#endif