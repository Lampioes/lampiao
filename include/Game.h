#ifndef GAME_H
#define GAME_H

#include <array>
#include <SDL2/SDL.h>
#include <box2d/box2d.h>
#include "../include/Player.h"

class Game
{
public:
    int run();

private:
    static constexpr float P2M = 30.0f;
    static constexpr int SCREEN_W = 1920;
    static constexpr int SCREEN_H = 1080;
    static constexpr int NUM_ZONES = 3;

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    b2World *world = nullptr;
    Player *player = nullptr;

    std::array<SDL_Texture *, NUM_ZONES> bgs{};
    std::array<int, NUM_ZONES> floorY{570, 570, 570};

    int cameraX = 0;
    bool running = true;

    bool init();
    bool initSDL();
    bool initPhysics();
    bool loadAssets();

    void handleEvents();
    void updateCamera();
    void render();
    void renderBackgrounds();
    void renderGround();
    void cleanup();
};

#endif