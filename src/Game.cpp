#include "../include/Game.h"

#include <SDL_image.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>

int Game::run() {
    if (!init()) {
        cleanup();
        return -1;
    }

    while (rodando) {
        handleEvents();
        update(1.0f / 60.0f);
        render();
    }

    cleanup();
    return 0;
}

bool Game::init() {
    return initSDL() && initAudio() && initPhysics() && loadAssets();
}

bool Game::initSDL() {
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

bool Game::initAudio() {
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

bool Game::initPhysics() {
    mundo = new b2World(b2Vec2(0.0f, GRAVIDADE));
    mundo->SetContactListener(&ouvinteContato);
    return true;
}

bool Game::loadAssets() {
    static constexpr const char* CAMINHOS_FUNDO[NUMERO_BACKGROUNDS] = {
        "../sprites/montanhas.jpg",
        "../sprites/transicao-montanhas-deserto.jpg",
        "../sprites/calica-deserto.png",
        "../sprites/cidade-deserto.jpeg",
        "../sprites/calica-deserto.png",
    };

    terreno = new Terrain(*mundo);

    const float jogadorInicioX = 300.0f;
    const float jogadorInicioY = terreno->getHeightAt(jogadorInicioX) - 80.0f;
    jogador = new Player(*mundo, renderizacao, jogadorInicioX, jogadorInicioY);

    for (int i = 0; i < NUMERO_BACKGROUNDS; ++i) {
        backgrounds[i] = IMG_LoadTexture(renderizacao, CAMINHOS_FUNDO[i]);
    }

    musicaFundo = Mix_LoadMUS("../audio/bg.ogg");
    somPulo = Mix_LoadWAV("../audio/jump.wav");

    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    Mix_VolumeChunk(somPulo, MIX_MAX_VOLUME / 2);

    setupLevel();
    return true;
}

void Game::setupLevel() {
    const float currumbaY = terreno->getHeightAt(PEN_X) - 30.0f;
    cercas.emplace_back(*mundo, PEN_X - 100.0f, currumbaY, 10.0f, 60.0f);
    cercas.emplace_back(*mundo, PEN_X + 100.0f, currumbaY, 10.0f, 60.0f);
    cercas.emplace_back(*mundo, PEN_X, currumbaY - 30.0f, 210.0f, 10.0f);

    const float larguraCiclo = static_cast<float>(Terrain::NUM_ZONAS * Terrain::TELA_W);
    const float espacoEntreVacas = larguraCiclo / NUMERO_VACAS;
    for (int i = 0; i < NUMERO_VACAS; ++i) {
        const float jitter = static_cast<float>((i * 137) % 200) - 100.0f;
        const float vacaX = espacoEntreVacas * (i + 0.5f) + jitter;
        const float vacaY = terreno->getHeightAt(vacaX) - 25.0f;
        vacas.emplace_back(*mundo, vacaX, vacaY, i);
    }
}

void Game::handleEvents() {
    SDL_Event evento;
    while (SDL_PollEvent(&evento)) {
        if (evento.type == SDL_QUIT) {
            rodando = false;
        }
        if (evento.type == SDL_KEYDOWN) {
            switch (evento.key.keysym.sym) {
            case SDLK_RIGHT:
                jogador->moveRight();
                break;
            case SDLK_LEFT:
                jogador->moveLeft();
                break;
            case SDLK_SPACE:
                jogador->jump();
                break;
            case SDLK_z:
                if (jogador->canShoot()) {
                    float px = jogador->getBody()->GetPosition().x * P2M;
                    float py = jogador->getBody()->GetPosition().y * P2M;
                    float dirX = jogador->getShootDirX();
                    float spawnX = px + dirX * 80.0f;
                    balas.emplace_back(*mundo, spawnX, py, dirX, 0.0f, true, renderizacao);
                    jogador->resetShootCooldown();
                }
                break;
            case SDLK_ESCAPE:
                rodando = false;
                break;
            }
        }
    }
}

void Game::update(float dt) {
    if (!mundo || !jogador || !terreno) {
        rodando = false;
        return;
    }

    mundo->Step(dt, 6, 2);

    jogador->update(dt);
    for (auto& bala : balas) {
        bala.update(dt);
    }

    const float jogadorPx = jogador->getBody()->GetPosition().x * P2M;
    for (auto& bandido : bandidos) {
        bandido.update(dt);

        if (bandido.shouldShoot(dt, jogadorPx)) {
            const float bx = bandido.getBody()->GetPosition().x * P2M;
            const float by = bandido.getBody()->GetPosition().y * P2M;
            const float dirX = bandido.getShootDirX(jogadorPx);
            balas.emplace_back(*mundo, bx + dirX * 30.0f, by, dirX, 0.0f, false, renderizacao);
        }
    }

    temporizadorSpawnBandido += dt;
    if (temporizadorSpawnBandido >= intervaloSpawnBandido) {
        spawnBandit();
        temporizadorSpawnBandido = 0.0f;
    }

    processCollisions();
    cleanupDead();
    updateCamera();

    if (!jogador->isAlive()) {
        rodando = false;
    }
}

void Game::updateCamera() {
    const int jogadorPixelX = static_cast<int>(jogador->getBody()->GetPosition().x * P2M);
    cameraX = jogadorPixelX - TELA_WIDTH / 2;
}

void Game::spawnBandit() {
    const float jogadorPx = jogador->getBody()->GetPosition().x * P2M;
    const float lado = (std::rand() % 2 == 0) ? 1.0f : -1.0f;
    const float spawnX = jogadorPx + lado * (TELA_WIDTH * 0.6f);
    const float spawnY = terreno->getHeightAt(spawnX) - 50.0f;

    bandidos.emplace_back(*mundo, renderizacao, spawnX, spawnY, proxIdBandido++);
}

void Game::processCollisions() {
    auto colisoes = ouvinteContato.getAndClearCollisions();

    for (auto& col : colisoes) {
        EntityData* a = col.a;
        EntityData* b = col.b;
        if (!a || !b) {
            continue;
        }

        if (a->tipo > b->tipo) {
            std::swap(a, b);
        }

        if (a->tipo == EntityType::BULLET_PLAYER && b->tipo == EntityType::BANDIT) {
            for (auto& bala : balas) {
                if (bala.isFromPlayer() && bala.isAlive()) {
                    bala.kill();
                    break;
                }
            }

            for (auto& bandido : bandidos) {
                if (bandido.getId() == b->id && bandido.isAlive()) {
                    bandido.takeDamage();
                    if (!bandido.isAlive()) {
                        pontuacao += 100;
                    }
                    break;
                }
            }
        }

        if (a->tipo == EntityType::PLAYER && b->tipo == EntityType::BULLET_BANDIT) {
            jogador->takeDamage();
            for (auto& bala : balas) {
                if (!bala.isFromPlayer() && bala.isAlive()) {
                    bala.kill();
                    break;
                }
            }
        }

        if (a->tipo == EntityType::PLAYER && b->tipo == EntityType::TERRAIN) jogador->setOnGround(true);

        if (b->tipo == EntityType::TERRAIN &&
            (a->tipo == EntityType::BULLET_PLAYER || a->tipo == EntityType::BULLET_BANDIT)) {
            for (auto& bala : balas) {
                if (bala.isAlive()) {
                    bala.kill();
                    break;
                }
            }
        }
    }
}

void Game::cleanupDead() {
    for (auto it = balas.begin(); it != balas.end();) {
        if (!it->isAlive()) {
            it->destroyBody(*mundo);
            it = balas.erase(it);
        } else ++it;
    }

    for (auto it = bandidos.begin(); it != bandidos.end();) {
        if (!it->isAlive()) {
            it->destroyBody(*mundo);
            it = bandidos.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderizacao, 135, 190, 230, 255);
    SDL_RenderClear(renderizacao);

    renderBackgrounds();

    terreno->draw(renderizacao, cameraX);
    for (auto& cerca : cercas) {
        cerca.draw(renderizacao, cameraX);
    }
    for (auto& vaca : vacas) {
        vaca.draw(renderizacao, cameraX);
    }
    for (auto& bandido : bandidos) {
        bandido.draw(renderizacao, cameraX);
    }
    for (auto& bala : balas) {
        bala.draw(renderizacao, cameraX);
    }

    jogador->draw(renderizacao, cameraX);
    renderHUD();

    SDL_RenderPresent(renderizacao);
}

void Game::renderBackgrounds() {
    const int zonaInicial = static_cast<int>(std::floor(static_cast<float>(cameraX) / TELA_WIDTH));

    for (int z = zonaInicial; z <= zonaInicial + 2; ++z) {
        const int telaX = z * TELA_WIDTH - cameraX;
        if (telaX + TELA_WIDTH < 0 || telaX > TELA_WIDTH) continue; 

        const int indiceFundo = ((z % NUMERO_BACKGROUNDS) + NUMERO_BACKGROUNDS) % NUMERO_BACKGROUNDS;
        SDL_Rect destino = {telaX, 0, TELA_WIDTH, TELA_ALTURA};

        if (backgrounds[indiceFundo]) {
            SDL_RenderCopy(renderizacao, backgrounds[indiceFundo], nullptr, &destino);
        }
    }
}

void Game::renderHUD() {
    SDL_SetRenderDrawColor(renderizacao, 220, 30, 30, 255);
    for (int i = 0; i < jogador->getHealth(); ++i) {
        SDL_Rect coracao = {20 + i * 30, 20, 24, 24};
        SDL_RenderFillRect(renderizacao, &coracao);
    }

    SDL_SetRenderDrawColor(renderizacao, 255, 215, 0, 255);
    const int larguraBarraPontuacao = std::min(pontuacao, 500);
    SDL_Rect barraPontuacao = {20, 55, larguraBarraPontuacao, 10};
    SDL_RenderFillRect(renderizacao, &barraPontuacao);
}

void Game::cleanup() {
    if (mundo) {
        for (auto& bala : balas) {
            bala.destroyBody(*mundo);
        }
        for (auto& bandido : bandidos) {
            bandido.destroyBody(*mundo);
        }
        for (auto& vaca : vacas) {
            vaca.destroyBody(*mundo);
        }
        for (auto& cerca : cercas) {
            cerca.destroyBody(*mundo);
        }
    }

    balas.clear();
    bandidos.clear();
    vacas.clear();
    cercas.clear();

    for (auto& bg : backgrounds) {
        if (bg) {
            SDL_DestroyTexture(bg);
            bg = nullptr;
        }
    }

    delete jogador;
    jogador = nullptr;

    delete terreno;
    terreno = nullptr;

    delete mundo;
    mundo = nullptr;

    if (somPulo) {
        Mix_FreeChunk(somPulo);
        somPulo = nullptr;
    }

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
