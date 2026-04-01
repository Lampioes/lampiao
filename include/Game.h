#ifndef GAME_H
#define GAME_H

#include <array>
#include <vector>
#include <SDL.h>
#include <box2d/box2d.h>
#include "Player.h"
#include "Terrain.h"
#include "Bullet.h"
#include "Cow.h"
#include "Bandit.h"
#include "Fence.h"
#include "Rope.h"
#include "ContactListener.h"

class Game {
public:
    int run();

private:
    static constexpr float P2M = 30.0f;
    static constexpr int SCREEN_W = 1920;
    static constexpr int SCREEN_H = 1080;
    static constexpr int NUM_ZONES = 5;
    static constexpr int NUM_COWS = 3;
    static constexpr float PEN_X = 500.0f;
    static constexpr float GRAVITY = 0.125f;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    b2World* world = nullptr;
    GameContactListener contactListener;
    Player* player = nullptr;
    Terrain* terrain = nullptr;

    std::vector<Bullet> bullets;
    std::vector<Bandit> bandits;
    std::vector<Cow> cows;
    std::vector<Fence> fences;

    std::array<SDL_Texture*, NUM_ZONES> bgs{};

    int cameraX = 0;
    bool running = true;
    float banditSpawnTimer = 0.0f;
    float banditSpawnInterval = 100.0f;
    int nextBanditId = 0;
    int score = 0;


    bool init();
    bool initSDL();
    bool initPhysics();
    bool loadAssets();
    void setupLevel();

    void handleEvents();
    void update(float dt);
    void updateCamera();
    void processCollisions();
    void spawnBandit();
    void cleanupDead();

    void render();
    void renderBackgrounds();
    void renderHUD();

    void cleanup();
};

#endif