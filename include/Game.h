#ifndef GAME_H
#define GAME_H

#include <array>
#include <vector>

#include <SDL.h>
#include <SDL_mixer.h>
#include <box2d/box2d.h>

#include "Bandit.h"
#include "Bullet.h"
#include "ContactListener.h"
#include "Cow.h"
#include "Fence.h"
#include "Player.h"
#include "Terrain.h"

class Game {
public:
    int run();

private:
    static constexpr float P2M = 30.0f;
    static constexpr int TELA_WIDTH = Terrain::SCREEN_W;
    static constexpr int TELA_ALTURA = Terrain::SCREEN_H;
    static constexpr int NUMERO_BACKGROUNDS = Terrain::NUM_ZONES;
    static constexpr int NUMERO_VACAS = 3;
    static constexpr float PEN_X = 500.0f;
    static constexpr float GRAVIDADE = 0.125f;

    SDL_Window* janela = nullptr;
    SDL_Renderer* renderizacao = nullptr;
    b2World* world = nullptr;
    GameContactListener contactListener;
    Player* jogador = nullptr;
    Terrain* terreno = nullptr;

    Mix_Music* bgMusic = nullptr;
    Mix_Chunk* jumpSound = nullptr;
    std::vector<Bullet> balas;
    std::vector<Bandit> bandidos;
    std::vector<Cow> vacas;
    std::vector<Fence> cercas;
    std::array<SDL_Texture*, NUMERO_BACKGROUNDS> bgs{};

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
