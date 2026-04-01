#include "../include/Game.h"

#include <cmath>
#include <SDL_image.h>
#include <algorithm>

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
    return initSDL() && initPhysics() && loadAssets();
}

bool Game::initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;

    const int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) return false;

    janela = SDL_CreateWindow("Lampioes 2D",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        TELA_WIDTH, TELA_AUTURA, 0);
    if (!janela) return false;

    renderizacao = SDL_CreateRenderer(janela, -1, SDL_RENDERER_ACCELERATED);
    if (!renderizacao) return false;

    SDL_SetRenderDrawBlendMode(renderizacao, SDL_BLENDMODE_BLEND);
    return true;
}

bool Game::initPhysics() {
    world = new b2World(b2Vec2(0.0f, GRAVIDADE));
    world->SetContactListener(&contactListener);
    return true;
}

bool Game::loadAssets() {
    terreno = new Terrain(*world);

    float playerStartX = 300.0f;
    float playerStartY = terreno->getHeightAt(playerStartX) - 80.0f;
    jogado = new Player(*world, renderizacao, playerStartX, playerStartY);
    
    bgs[0] = IMG_LoadTexture(renderizacao, "../sprites/montanhas.jpg");
    bgs[1] = IMG_LoadTexture(renderizacao, "../sprites/transicao-montanhas-deserto.jpg");
    bgs[2] = IMG_LoadTexture(renderizacao, "../sprites/calica-deserto.png");
    bgs[3] = IMG_LoadTexture(renderizacao, "../sprites/cidade-deserto.png");
    bgs[4] = IMG_LoadTexture(renderizacao, "../sprites/calica-deserto.png");

    setupLevel();
    return true;
}

void Game::setupLevel() {
    float penY = terreno->getHeightAt(PEN_X) - 30.0f;
    cercas.emplace_back(*world, PEN_X - 100.0f, penY, 10.0f, 60.0f); 
    cercas.emplace_back(*world, PEN_X + 100.0f, penY, 10.0f, 60.0f); 
    cercas.emplace_back(*world, PEN_X, penY - 30.0f, 210.0f, 10.0f); 

    for (int i = 0; i < NUMERO_VACAS; ++i) {
        float cowX = PEN_X - 50.0f + i * 50.0f;
        float cowY = terreno->getHeightAt(cowX) - 25.0f;
        vacas.emplace_back(*world, cowX, cowY, i);
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
            case SDLK_RIGHT:
                jogado->moveRight();
                break;
            case SDLK_LEFT:
                jogado->moveLeft();
                break;
            case SDLK_SPACE:
                jogado->jump();
                break;
            case SDLK_z:
                if (jogado->canShoot()) {
                    float px = jogado->getBody()->GetPosition().x * P2M;
                    float py = jogado->getBody()->GetPosition().y * P2M;
                    float dirX = jogado->getShootDirX();
                    float spawnX = px + dirX * 80.0f;
                    balas.emplace_back(*world, spawnX, py, dirX, 0.0f, true);
                    jogado->resetShootCooldown();
                }
                break;
            case SDLK_ESCAPE:
                running = false;
                break;
            }
        }
    }
}

void Game::update(float dt) {
    world->Step(dt, 6, 2);

    jogado->update(dt);

    for (auto& b : balas) b.update(dt);

    float playerPx = jogado->getBody()->GetPosition().x * P2M;
    for (auto& bandit : bandidos) {
        bandit.update(dt);

        if (bandit.shouldShoot(dt)) {
            float bx = bandit.getBody()->GetPosition().x * P2M;
            float by = bandit.getBody()->GetPosition().y * P2M;
            float dirX = bandit.getShootDirX(playerPx);
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
}

void Game::updateCamera() {
    int playerPixelX = static_cast<int>(jogado->getBody()->GetPosition().x * P2M);
    cameraX = playerPixelX - TELA_WIDTH / 2;
}

void Game::spawnBandit() {
    float playerPx = jogado->getBody()->GetPosition().x * P2M;
    float side = (rand() % 2 == 0) ? 1.0f : -1.0f;
    float spawnX = playerPx + side * (TELA_WIDTH * 0.6f);
    float spawnY = terreno->getHeightAt(spawnX) - 50.0f;

    bandidos.emplace_back(*world, renderizacao, spawnX, spawnY, nextBanditId++);
}

void Game::processCollisions() {
    auto collisions = contactListener.getAndClearCollisions();

    for (auto& col : collisions) {
        EntityData* a = col.a;
        EntityData* b = col.b;

        if (a->type > b->type) std::swap(a, b);

        if (a->type == EntityType::BULLET_PLAYER && b->type == EntityType::BANDIT) {
            for (auto& bullet : balas) {
                if (bullet.isFromPlayer() && bullet.isAlive()) {
                    bullet.kill();
                    break;
                }
            }
            for (auto& bandit : bandidos) {
                if (bandit.getId() == b->id && bandit.isAlive()) {
                    bandit.takeDamage();
                    if (!bandit.isAlive()) score += 100;
                    break;
                }
            }
        }

        if (a->type == EntityType::PLAYER && b->type == EntityType::BULLET_BANDIT) {
            jogado->takeDamage();
            for (auto& bullet : balas) {
                if (!bullet.isFromPlayer() && bullet.isAlive()) {
                    bullet.kill();
                    break;
                }
            }
        }

        if (b->type == EntityType::TERRAIN) {
            if (a->type == EntityType::BULLET_PLAYER || a->type == EntityType::BULLET_BANDIT) {
                for (auto& bullet : balas) {
                    if (bullet.isAlive()) {
                        bullet.kill();
                        break;
                    }
                }
            }
            if ((a->type == EntityType::PLAYER && b->type == EntityType::TERRAIN) ||
                (a->type == EntityType::TERRAIN && b->type == EntityType::PLAYER)) {
                            jogado->setOnGround(true);
                }
        }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderizacao, 135, 190, 230, 255); 
    SDL_RenderClear(renderizacao);

    renderBackgrounds();

    terreno->draw(renderizacao, cameraX);

    for (auto& fence : cercas) {
        fence.draw(renderizacao, cameraX);
    }
    for (auto& cow : vacas) {
        cow.draw(renderizacao, cameraX);
    }

    for (auto& bandit : bandidos) {
        bandit.draw(renderizacao, cameraX);
    }

    for (auto& bullet : balas) {
        bullet.draw(renderizacao, cameraX);
    }

    jogado->draw(renderizacao, cameraX);

    renderHUD();

    SDL_RenderPresent(renderizacao);
}

void Game::renderBackgrounds() {
    int startZone = static_cast<int>(std::floor(static_cast<float>(cameraX) / TELA_WIDTH));

    for (int z = startZone; z <= startZone + 2; ++z) {
        int screenX = z * TELA_WIDTH - cameraX;
        if (screenX + TELA_WIDTH < 0 || screenX > TELA_WIDTH) continue;

        int bgIndex = ((z % NUMER_BACKGROUNDS) + NUMER_BACKGROUNDS) % NUMER_BACKGROUNDS;
        SDL_Rect dst = {screenX, 0, TELA_WIDTH, TELA_AUTURA};

        if (bgs[bgIndex]) {
            SDL_RenderCopy(renderizacao, bgs[bgIndex], nullptr, &dst);
        }
    }
}

void Game::renderHUD() {
    SDL_SetRenderDrawColor(renderizacao, 220, 30, 30, 255);
    for (int i = 0; i < jogado->getHealth(); ++i) {
        SDL_Rect heart = {20 + i * 30, 20, 24, 24};
        SDL_RenderFillRect(renderizacao, &heart);
    }

    SDL_SetRenderDrawColor(renderizacao, 255, 215, 0, 255);
    int scoreBarW = std::min(score, 500);
    SDL_Rect scoreBar = {20, 55, scoreBarW, 10};
    SDL_RenderFillRect(renderizacao, &scoreBar);
}

void Game::cleanupDead() {
    for (auto it = balas.begin(); it != balas.end(); ) {
        if (!it->isAlive()) {
            it->destroyBody(*world);
            it = balas.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = bandidos.begin(); it != bandidos.end(); ) {
        if (!it->isAlive()) {
            it->destroyBody(*world);
            it = bandidos.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::cleanup() {
    for (auto& b : balas)  b.destroyBody(*world);
    for (auto& b : bandidos)  b.destroyBody(*world);
    for (auto& c : vacas)     c.destroyBody(*world);
    for (auto& f : cercas)   f.destroyBody(*world);

    balas.clear();
    bandidos.clear();
    vacas.clear();
    cercas.clear();

    delete jogado;
    jogado = nullptr;

    delete terreno;
    terreno = nullptr;

    delete world;
    world = nullptr;

    for (int i = 0; i < NUMER_BACKGROUNDS; ++i) {
        if (bgs[i]) {
            SDL_DestroyTexture(bgs[i]);
            bgs[i] = nullptr;
        }
    }

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