#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#include <memory>
#include <vector>

#include <SDL.h>
#include <SDL_mixer.h>
#include <box2d/box2d.h>

#include "Camera.h"
#include "ContactListener.h"
#include "GameObject.h"
#include "Player.h"
#include "Scene.h"
#include "Terrain.h"

class GameWorld : public Scene {
public:
    explicit GameWorld(const ContextoJogo& contexto);
    ~GameWorld() override;

    void handleEvent(const SDL_Event& evento) override;
    void update(float dt) override;
    void draw() override;

private:
    static constexpr float PIXELSPORMETRO = 30.0f;
    static constexpr int NUMERO_BACKGROUNDS = Terrain::NUM_ZONAS;
    static constexpr int NUMERO_VACAS = 10;
    static constexpr float GRAVIDADE = 10.0f;
    static constexpr int MAX_BANDIDOS_VIVOS = 3;

    std::unique_ptr<b2World> mundo;
    GameContactListener ouvinteContato;

    std::vector<std::unique_ptr<GameObject>> objetos;
    std::vector<std::unique_ptr<GameObject>> paraAdicionar;
    Player* jogador = nullptr;
    Terrain* terreno = nullptr;

    Mix_Chunk* somPulo = nullptr;
    Mix_Chunk* somTiro = nullptr;

    Camera camera;
    float temporizadorSpawnBandido = 0.0f;
    float intervaloSpawnBandido = 5.0f;
    int proxIdBandido = 0;
    int pontuacao = 0;
    bool terminado = false;
    bool venceu = false;

    void loadSounds();
    void setupLevel();

    void updateCamera();
    void processCollisions();
    void handlePlayerActions();
    void spawnBandit();
    void cleanupDead();
    void flushSpawns();

    void renderBackgrounds();
    void renderHUD();
    void renderFimDeJogo();

    template <typename T, typename Fn>
    void forEach(Fn&& fn) {
        for (auto& o : objetos) {
            if (auto* t = dynamic_cast<T*>(o.get())) fn(*t);
        }
    }
};

#endif
