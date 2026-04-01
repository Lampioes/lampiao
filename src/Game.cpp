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

    while (running) {
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

    const int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
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
    world = new b2World(b2Vec2(0.0f, GRAVIDADE));
    world->SetContactListener(&contactListener);
    return true;
}

bool Game::loadAssets() {
    static constexpr const char* BACKGROUND_PATHS[NUMERO_BACKGROUNDS] = {
        "../sprites/montanhas.jpg",
        "../sprites/transicao-montanhas-deserto.jpg",
        "../sprites/calica-deserto.png",
        "../sprites/cidade-deserto.jpeg",
        "../sprites/calica-deserto.png",
    };

    terreno = new Terrain(*world);

    const float playerStartX = 300.0f;
    const float playerStartY = terreno->getHeightAt(playerStartX) - 80.0f;
    jogador = new Player(*world, renderizacao, playerStartX, playerStartY);

    for (int i = 0; i < NUMERO_BACKGROUNDS; ++i) {
        bgs[i] = IMG_LoadTexture(renderizacao, BACKGROUND_PATHS[i]);
        if (!bgs[i]) {
            SDL_Log("Erro ao carregar background %d: %s", i, IMG_GetError());
            return false;
        }
    }

    bgMusic = Mix_LoadMUS("../audio/bg.ogg");
    if (!bgMusic) {
        SDL_Log("Erro ao carregar musica: %s", Mix_GetError());
        return false;
    }

    jumpSound = Mix_LoadWAV("../audio/jump.wav");
    if (!jumpSound) {
        SDL_Log("Erro ao carregar efeito de pulo: %s", Mix_GetError());
        return false;
    }

    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    Mix_VolumeChunk(jumpSound, MIX_MAX_VOLUME / 2);

    if (Mix_PlayMusic(bgMusic, -1) < 0) {
        SDL_Log("Erro ao tocar musica: %s", Mix_GetError());
        return false;
    }

    setupLevel();
    return true;
}

void Game::setupLevel() {
    const float penY = terreno->getHeightAt(PEN_X) - 30.0f;
    cercas.emplace_back(*world, PEN_X - 100.0f, penY, 10.0f, 60.0f);
    cercas.emplace_back(*world, PEN_X + 100.0f, penY, 10.0f, 60.0f);
    cercas.emplace_back(*world, PEN_X, penY - 30.0f, 210.0f, 10.0f);

    for (int i = 0; i < NUMERO_VACAS; ++i) {
        const float cowX = PEN_X - 50.0f + static_cast<float>(i) * 50.0f;
        const float cowY = terreno->getHeightAt(cowX) - 25.0f;
        vacas.emplace_back(*world, cowX, cowY, i);
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
            continue;
        }

        if (event.type != SDL_KEYDOWN) {
            continue;
        }

        switch (event.key.keysym.sym) {
        case SDLK_RIGHT:
            jogador->moveRight();
            break;
        case SDLK_LEFT:
            jogador->moveLeft();
            break;
        case SDLK_SPACE:
            jogador->jump();
            if (jumpSound) {
                Mix_PlayChannel(-1, jumpSound, 0);
            }
            break;
        case SDLK_z:
            if (jogador->canShoot()) {
                const float px = jogador->getBody()->GetPosition().x * P2M;
                const float py = jogador->getBody()->GetPosition().y * P2M;
                const float dirX = jogador->getShootDirX();
                const float spawnX = px + dirX * 80.0f;
                balas.emplace_back(*world, spawnX, py, dirX, 0.0f, true);
                jogador->resetShootCooldown();
            }
            break;
        case SDLK_ESCAPE:
            running = false;
            break;
        default:
            break;
        }
    }
}

void Game::update(float dt) {
    if (!world || !jogador || !terreno) {
        running = false;
        return;
    }

    world->Step(dt, 6, 2);

    jogador->update(dt);
    for (auto& bala : balas) {
        bala.update(dt);
    }

    const float playerPx = jogador->getBody()->GetPosition().x * P2M;
    for (auto& bandit : bandidos) {
        bandit.update(dt);

        if (bandit.shouldShoot(dt)) {
            const float bx = bandit.getBody()->GetPosition().x * P2M;
            const float by = bandit.getBody()->GetPosition().y * P2M;
            const float dirX = bandit.getShootDirX(playerPx);
            balas.emplace_back(*world, bx + dirX * 30.0f, by, dirX, 0.0f, false);
        }
    }

    banditSpawnTimer += dt;
    if (banditSpawnTimer >= banditSpawnInterval) {
        spawnBandit();
        banditSpawnTimer = 0.0f;
    }

    processCollisions();
    cleanupDead();
    updateCamera();

    if (!jogador->isAlive()) {
        running = false;
    }
}

void Game::updateCamera() {
    const int playerPixelX = static_cast<int>(jogador->getBody()->GetPosition().x * P2M);
    cameraX = playerPixelX - TELA_WIDTH / 2;
}

void Game::spawnBandit() {
    const float playerPx = jogador->getBody()->GetPosition().x * P2M;
    const float side = (std::rand() % 2 == 0) ? 1.0f : -1.0f;
    const float spawnX = playerPx + side * (TELA_WIDTH * 0.6f);
    const float spawnY = terreno->getHeightAt(spawnX) - 50.0f;

    bandidos.emplace_back(*world, renderizacao, spawnX, spawnY, nextBanditId++);
}

void Game::processCollisions() {
    auto collisions = contactListener.getAndClearCollisions();

    for (auto& col : collisions) {
        EntityData* a = col.a;
        EntityData* b = col.b;
        if (!a || !b) {
            continue;
        }

        if (a->type > b->type) {
            std::swap(a, b);
        }

        if (a->type == EntityType::BULLET_PLAYER && b->type == EntityType::BANDIT) {
            for (auto& bala : balas) {
                if (bala.isFromPlayer() && bala.isAlive()) {
                    bala.kill();
                    break;
                }
            }

            for (auto& bandit : bandidos) {
                if (bandit.getId() == b->id && bandit.isAlive()) {
                    bandit.takeDamage();
                    if (!bandit.isAlive()) {
                        score += 100;
                    }
                    break;
                }
            }
        }

        if (a->type == EntityType::PLAYER && b->type == EntityType::BULLET_BANDIT) {
            jogador->takeDamage();
            for (auto& bala : balas) {
                if (!bala.isFromPlayer() && bala.isAlive()) {
                    bala.kill();
                    break;
                }
            }
        }

        if (a->type == EntityType::PLAYER && b->type == EntityType::TERRAIN) {
            jogador->setOnGround(true);
        }

        if (b->type == EntityType::TERRAIN &&
            (a->type == EntityType::BULLET_PLAYER || a->type == EntityType::BULLET_BANDIT)) {
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
            it->destroyBody(*world);
            it = balas.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = bandidos.begin(); it != bandidos.end();) {
        if (!it->isAlive()) {
            it->destroyBody(*world);
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
    for (auto& bandit : bandidos) {
        bandit.draw(renderizacao, cameraX);
    }
    for (auto& bala : balas) {
        bala.draw(renderizacao, cameraX);
    }

    jogador->draw(renderizacao, cameraX);
    renderHUD();

    SDL_RenderPresent(renderizacao);
}

void Game::renderBackgrounds() {
    const int startZone = static_cast<int>(std::floor(static_cast<float>(cameraX) / TELA_WIDTH));

    for (int z = startZone; z <= startZone + 2; ++z) {
        const int screenX = z * TELA_WIDTH - cameraX;
        if (screenX + TELA_WIDTH < 0 || screenX > TELA_WIDTH) {
            continue;
        }

        const int bgIndex = ((z % NUMERO_BACKGROUNDS) + NUMERO_BACKGROUNDS) % NUMERO_BACKGROUNDS;
        SDL_Rect dst = {screenX, 0, TELA_WIDTH, TELA_ALTURA};

        if (bgs[bgIndex]) {
            SDL_RenderCopy(renderizacao, bgs[bgIndex], nullptr, &dst);
        }
    }
}

void Game::renderHUD() {
    SDL_SetRenderDrawColor(renderizacao, 220, 30, 30, 255);
    for (int i = 0; i < jogador->getHealth(); ++i) {
        SDL_Rect heart = {20 + i * 30, 20, 24, 24};
        SDL_RenderFillRect(renderizacao, &heart);
    }

    SDL_SetRenderDrawColor(renderizacao, 255, 215, 0, 255);
    const int scoreBarW = std::min(score, 500);
    SDL_Rect scoreBar = {20, 55, scoreBarW, 10};
    SDL_RenderFillRect(renderizacao, &scoreBar);
}

void Game::cleanup() {
    if (world) {
        for (auto& bala : balas) {
            bala.destroyBody(*world);
        }
        for (auto& bandit : bandidos) {
            bandit.destroyBody(*world);
        }
        for (auto& vaca : vacas) {
            vaca.destroyBody(*world);
        }
        for (auto& cerca : cercas) {
            cerca.destroyBody(*world);
        }
    }

    balas.clear();
    bandidos.clear();
    vacas.clear();
    cercas.clear();

    for (auto& bg : bgs) {
        if (bg) {
            SDL_DestroyTexture(bg);
            bg = nullptr;
        }
    }

    delete jogador;
    jogador = nullptr;

    delete terreno;
    terreno = nullptr;

    delete world;
    world = nullptr;

    if (jumpSound) {
        Mix_FreeChunk(jumpSound);
        jumpSound = nullptr;
    }

    if (bgMusic) {
        Mix_FreeMusic(bgMusic);
        bgMusic = nullptr;
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
