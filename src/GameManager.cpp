#include "../include/GameManager.h"

#include <SDL_image.h>

#include "../include/GameWorld.h"
#include "../include/MenuScene.h"

namespace {

// primeira fonte que existir na maquina (a pasta fonts do projeto vem primeiro)
const char* const CAMINHOS_FONTE[] = {
    "../fonts/fonte.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
    "/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf",
    "/usr/share/fonts/truetype/freefont/FreeSansBold.ttf",
    "/usr/share/fonts/truetype/noto/NotoSans-Bold.ttf",
};

}  // namespace

int GameManager::run() {
    if (!init()) {
        cleanup();
        return -1;
    }

    aplicarPedido(PedidoCena::trocar(TipoCena::MENU));

    const Uint64 frequencia = SDL_GetPerformanceFrequency();
    Uint64 instanteAnterior = SDL_GetPerformanceCounter();

    while (rodando) {
        const Uint64 instanteAtual = SDL_GetPerformanceCounter();
        float delta_t = static_cast<float>(instanteAtual - instanteAnterior) / static_cast<float>(frequencia);
        instanteAnterior = instanteAtual;

        if (delta_t > 0.1f) delta_t = 0.1f;

        handleEvents();
        update(delta_t);
        render();
    }

    cleanup();
    return 0;
}

bool GameManager::init() {
    if (!initSDL()) return false;
    if (!initAudio()) return false;
    if (!initFontes()) return false;
    loadAssets();
    return true;
}

bool GameManager::initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        SDL_Log("Erro ao iniciar SDL: %s", SDL_GetError());
        return false;
    }

    const int flagsImagem = IMG_INIT_JPG | IMG_INIT_PNG;
    if ((IMG_Init(flagsImagem) & flagsImagem) != flagsImagem) {
        SDL_Log("Erro ao iniciar SDL_image: %s", IMG_GetError());
        return false;
    }

    janela = SDL_CreateWindow(
        "Lampioes 2D",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        TELA_WIDTH,
        TELA_ALTURA,
        0);
    if (!janela) {
        SDL_Log("Erro ao criar janela: %s", SDL_GetError());
        return false;
    }

    renderizacao = SDL_CreateRenderer(janela, -1, SDL_RENDERER_ACCELERATED);
    if (!renderizacao) {
        SDL_Log("Erro ao criar renderer: %s", SDL_GetError());
        return false;
    }

    SDL_SetRenderDrawBlendMode(renderizacao, SDL_BLENDMODE_BLEND);
    return true;
}

bool GameManager::initAudio() {
    if ((Mix_Init(MIX_INIT_OGG) & MIX_INIT_OGG) != MIX_INIT_OGG) {
        SDL_Log("Erro ao iniciar SDL_mixer: %s", Mix_GetError());
        return false;
    }

    if (Mix_OpenAudio(48000, AUDIO_S16SYS, 2, 2048) < 0) {
        SDL_Log("Erro ao abrir audio: %s", Mix_GetError());
        return false;
    }

    Mix_AllocateChannels(16);
    return true;
}

bool GameManager::initFontes() {
    if (TTF_Init() < 0) {
        SDL_Log("Erro ao iniciar SDL_ttf: %s", TTF_GetError());
        return false;
    }

    for (const char* caminho : CAMINHOS_FONTE) {
        fonteTitulo = TTF_OpenFont(caminho, 90);
        if (!fonteTitulo) continue;

        fonteMenu = TTF_OpenFont(caminho, 48);
        fonteHud = TTF_OpenFont(caminho, 26);
        break;
    }

    if (!fonteTitulo || !fonteMenu || !fonteHud) {
        SDL_Log("Erro ao abrir a fonte: %s", TTF_GetError());
        return false;
    }

    return true;
}

void GameManager::loadAssets() {
    sprites.loadAll(renderizacao);

    musicaFundo = Mix_LoadMUS("../audio/bg.ogg");
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    if (musicaFundo) Mix_PlayMusic(musicaFundo, -1);
}

ContextoJogo GameManager::montarContexto() const {
    ContextoJogo contexto;
    contexto.renderizacao = renderizacao;
    contexto.sprites = &sprites;
    contexto.fonteTitulo = fonteTitulo;
    contexto.fonteMenu = fonteMenu;
    contexto.fonteHud = fonteHud;
    contexto.telaLargura = TELA_WIDTH;
    contexto.telaAltura = TELA_ALTURA;
    return contexto;
}

std::unique_ptr<Scene> GameManager::criarCena(TipoCena tipo) {
    if (tipo == TipoCena::JOGO) {
        return std::make_unique<GameWorld>(montarContexto());
    }
    return std::make_unique<MenuScene>(montarContexto(), tipo);
}

void GameManager::aplicarPedido(const PedidoCena& pedido) {
    switch (pedido.acao) {
    case AcaoCena::EMPILHAR:
        pilha.push_back(criarCena(pedido.cena));
        break;

    case AcaoCena::DESEMPILHAR:
        if (!pilha.empty()) pilha.pop_back();
        if (pilha.empty()) rodando = false;
        break;

    case AcaoCena::TROCAR:
        // descarrega tudo que estava aberto antes de montar a cena nova
        pilha.clear();
        pilha.push_back(criarCena(pedido.cena));
        break;

    case AcaoCena::SAIR:
        rodando = false;
        break;

    case AcaoCena::NENHUMA:
        break;
    }
}

void GameManager::handleEvents() {
    SDL_Event evento;
    while (SDL_PollEvent(&evento)) {
        if (evento.type == SDL_QUIT) {
            rodando = false;
            return;
        }
        if (Scene* cena = topo()) cena->handleEvent(evento);
    }
}

void GameManager::update(float dt) {
    Scene* cena = topo();
    if (!cena) {
        rodando = false;
        return;
    }

    cena->update(dt);

    const PedidoCena pedido = cena->getPedido();
    if (pedido.acao != AcaoCena::NENHUMA) {
        cena->limparPedido();
        aplicarPedido(pedido);
    }
}

void GameManager::render() {
    SDL_SetRenderDrawColor(renderizacao, 135, 190, 230, 255);
    SDL_RenderClear(renderizacao);

    // acha a ultima cena opaca e desenha dali pro topo, assim os menus
    // transparentes aparecem em cascata por cima do que tem embaixo
    size_t primeira = 0;
    for (size_t i = pilha.size(); i > 0; --i) {
        if (!pilha[i - 1]->transparente()) {
            primeira = i - 1;
            break;
        }
    }

    for (size_t i = primeira; i < pilha.size(); ++i) {
        pilha[i]->draw();
    }

    SDL_RenderPresent(renderizacao);
}

void GameManager::cleanup() {
    pilha.clear();

    sprites.clear();

    if (fonteTitulo) {
        TTF_CloseFont(fonteTitulo);
        fonteTitulo = nullptr;
    }
    if (fonteMenu) {
        TTF_CloseFont(fonteMenu);
        fonteMenu = nullptr;
    }
    if (fonteHud) {
        TTF_CloseFont(fonteHud);
        fonteHud = nullptr;
    }
    TTF_Quit();

    if (musicaFundo) {
        Mix_FreeMusic(musicaFundo);
        musicaFundo = nullptr;
    }

    Mix_CloseAudio();
    Mix_Quit();

    if (renderizacao) {
        SDL_DestroyRenderer(renderizacao);
        renderizacao = nullptr;
    }
    if (janela) {
        SDL_DestroyWindow(janela);
        janela = nullptr;
    }

    IMG_Quit();
    SDL_Quit();
}
