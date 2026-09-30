#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <memory>
#include <vector>

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include "Scene.h"
#include "SpriteCatalog.h"
#include "Terrain.h"

class GameManager {
public:
    int run();

private:
    static constexpr int TELA_WIDTH = Terrain::TELA_W;
    static constexpr int TELA_ALTURA = Terrain::TELA_H;

    SDL_Window* janela = nullptr;
    SDL_Renderer* renderizacao = nullptr;
    Mix_Music* musicaFundo = nullptr;

    SpriteCatalog sprites;
    TTF_Font* fonteTitulo = nullptr;
    TTF_Font* fonteMenu = nullptr;
    TTF_Font* fonteHud = nullptr;

    std::vector<std::unique_ptr<Scene>> pilha;

    bool rodando = true;

    bool init();
    bool initSDL();
    bool initAudio();
    bool initFontes();
    void loadAssets();

    ContextoJogo montarContexto() const;
    std::unique_ptr<Scene> criarCena(TipoCena tipo);
    void aplicarPedido(const PedidoCena& pedido);
    Scene* topo() const { return pilha.empty() ? nullptr : pilha.back().get(); }

    void handleEvents();
    void update(float dt);
    void render();
    void cleanup();
};

#endif
