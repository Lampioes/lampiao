#ifndef GAME_H
#define GAME_H

#include <array>
#include <memory>
#include <vector>

#include <SDL.h>
#include <SDL_mixer.h>
#include <box2d/box2d.h>

#include "ContactListener.h"
#include "GameObject.h"
#include "Player.h"
#include "Terrain.h"

class Game {
public:
    int run();

private:
    static constexpr float PIXELSPORMETRO = 30.0f;
    static constexpr int TELA_WIDTH = Terrain::TELA_W;
    static constexpr int TELA_ALTURA = Terrain::TELA_H;
    static constexpr int NUMERO_BACKGROUNDS = Terrain::NUM_ZONAS;
    static constexpr int NUMERO_VACAS = 10;
    static constexpr float PEN_X = 500.0f;
    static constexpr float GRAVIDADE = 10.0f;

    SDL_Window* janela = nullptr;
    SDL_Renderer* renderizacao = nullptr;
    b2World* mundo = nullptr;
    GameContactListener ouvinteContato;

    // todo: desmisturar isso e colocar na game manager
    // todo: criar game manager
    // na hora de fazer menu precisamos fazer com que a game manager carregue o menu
    // dai dps descarregar o game manager e carregar o game world
    // os eventos tudo vão ser da game manager - eventos de teclado, mouse etc e repassar os eventos pros objetos
    // podemos ter gerencia de evento separados e os objetos que querem ter um gerencia de eventos se inscrever nele (mais dificil)
    std::vector<std::unique_ptr<GameObject>> objetos;
    std::vector<std::unique_ptr<GameObject>> paraAdicionar;
    Player* jogador = nullptr;
    Terrain* terreno = nullptr;

    Mix_Music* musicaFundo = nullptr;
    Mix_Chunk* somPulo = nullptr;
    Mix_Chunk* somTiro = nullptr;
    std::array<SDL_Texture*, NUMERO_BACKGROUNDS> backgrounds{};

    //todo: colocar camera como vector x e y
    int cameraX = 0;
    bool rodando = true;
    float temporizadorSpawnBandido = 0.0f;
    float intervaloSpawnBandido = 5.0f;
    int proxIdBandido = 0;
    static constexpr int MAX_BANDIDOS_VIVOS = 3;
    int pontuacao = 0;

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
    void flushSpawns();

    template <typename T, typename Fn>
    void forEach(Fn&& fn) {
        for (auto& o : objetos) {
            if (auto* t = dynamic_cast<T*>(o.get())) fn(*t);
        }
    }

    void render();
    void renderBackgrounds();
    void renderHUD();
    void cleanup();
};

#endif
