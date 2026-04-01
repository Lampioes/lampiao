#ifndef GAME_H
#define GAME_H

#include <array>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
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
    static constexpr int TELA_WIDTH = 1920;
    static constexpr int TELA_AUTURA = 1080;
    static constexpr int NUMER_BACKGROUNDS = 5;
    static constexpr int NUMERO_VACAS = 3;
    static constexpr int NUMERO_BANDIDOS = 5;
    static constexpr float PEN_X = 500.0f;
    static constexpr float GRAVIDADE = 0.125f;

    SDL_Window* janela = nullptr;
    SDL_Renderer* renderizacao = nullptr;
    b2World* world = nullptr;
    GameContactListener contactListener;
    Player* jogado = nullptr;
    Terrain* terreno = nullptr;

    std::array<SDL_Texture *, NUM_ZONES> bgs{};
    std::array<int, NUM_ZONES> floorY{570, 570, 570};

    Mix_Music *bgMusic = nullptr;
    Mix_Chunk *jumpSound = nullptr;
    std::vector<Bullet> bala;
    std::vector<Bandit> bandidos;
    std::vector<Cow> vacas;
    std::vector<Fence> cercas;

    std::array<SDL_Texture*, NUMER_BACKGROUNDS> bgs{};

    int cameraX = 0;
    bool running = true;
    float banditSpawnTimer = 0.0f;
    float banditSpawnInterval = 100.0f;
    int nextBanditId = 0;
    int score = 0;


    bool init();
    bool initSDL();
    bool initAudio();
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