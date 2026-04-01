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

    window = SDL_CreateWindow("Lampioes 2D",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_W, SCREEN_H, 0);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    return true;
}

bool Game::initPhysics() {
    world = new b2World(b2Vec2(0.0f, GRAVITY));
    world->SetContactListener(&contactListener);
    return true;
}

bool Game::loadAssets() {
    terrain = new Terrain(*world);

    float playerStartX = 300.0f;
    float playerStartY = terrain->getHeightAt(playerStartX) - 80.0f;
    player = new Player(*world, renderer, playerStartX, playerStartY);
    
    bgs[0] = IMG_LoadTexture(renderer, "../sprites/montanhas.jpg");
    bgs[1] = IMG_LoadTexture(renderer, "../sprites/transicao-montanhas-deserto.jpg");
    bgs[2] = IMG_LoadTexture(renderer, "../sprites/calica-deserto.jpg");
    bgs[3] = IMG_LoadTexture(renderer, "../sprites/cidade-deserto.jpg");
    bgs[4] = IMG_LoadTexture(renderer, "../sprites/calica-deserto.jpg");

    setupLevel();
    return true;
}

void Game::setupLevel() {
    float penY = terrain->getHeightAt(PEN_X) - 30.0f;
    fences.emplace_back(*world, PEN_X - 100.0f, penY, 10.0f, 60.0f); 
    fences.emplace_back(*world, PEN_X + 100.0f, penY, 10.0f, 60.0f); 
    fences.emplace_back(*world, PEN_X, penY - 30.0f, 210.0f, 10.0f); 

    for (int i = 0; i < NUM_COWS; ++i) {
        float cowX = PEN_X - 50.0f + i * 50.0f;
        float cowY = terrain->getHeightAt(cowX) - 25.0f;
        cows.emplace_back(*world, cowX, cowY, i);
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
                player->moveRight();
                break;
            case SDLK_LEFT:
                player->moveLeft();
                break;
            case SDLK_SPACE:
                player->jump();
                break;
            case SDLK_z:
                if (player->canShoot()) {
                    float px = player->getBody()->GetPosition().x * P2M;
                    float py = player->getBody()->GetPosition().y * P2M;
                    float dirX = player->getShootDirX();
                    float spawnX = px + dirX * 80.0f;
                    bullets.emplace_back(*world, spawnX, py, dirX, 0.0f, true, renderer);
                    player->resetShootCooldown();
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

    player->update(dt);

    for (auto& b : bullets) {
        b.update(dt);
    }

    float playerPx = player->getBody()->GetPosition().x * P2M;
    for (auto& bandit : bandits) {
        bandit.update(dt);

        if (bandit.shouldShoot(dt)) {
            float bx = bandit.getBody()->GetPosition().x * P2M;
            float by = bandit.getBody()->GetPosition().y * P2M;
            float dirX = bandit.getShootDirX(playerPx);
            bullets.emplace_back(*world, bx + dirX * 30.0f, by, dirX, 0.0f, false, renderer);
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
    int playerPixelX = static_cast<int>(player->getBody()->GetPosition().x * P2M);
    cameraX = playerPixelX - SCREEN_W / 2;
}

void Game::spawnBandit() {
    float playerPx = player->getBody()->GetPosition().x * P2M;
    float side = (rand() % 2 == 0) ? 1.0f : -1.0f;
    float spawnX = playerPx + side * (SCREEN_W * 0.6f);
    float spawnY = terrain->getHeightAt(spawnX) - 50.0f;

    bandits.emplace_back(*world, renderer, spawnX, spawnY, nextBanditId++);
}

void Game::processCollisions() {
    auto collisions = contactListener.getAndClearCollisions();

    for (auto& col : collisions) {
        EntityData* a = col.a;
        EntityData* b = col.b;

        if (a->type > b->type) std::swap(a, b);

        if (a->type == EntityType::BULLET_PLAYER && b->type == EntityType::BANDIT) {
            for (auto& bullet : bullets) {
                if (bullet.isFromPlayer() && bullet.isAlive()) {
                    bullet.kill();
                    break;
                }
            }
            for (auto& bandit : bandits) {
                if (bandit.getId() == b->id && bandit.isAlive()) {
                    bandit.takeDamage();
                    if (!bandit.isAlive()) score += 100;
                    break;
                }
            }
        }

        if (a->type == EntityType::PLAYER && b->type == EntityType::BULLET_BANDIT) {
            player->takeDamage();
            for (auto& bullet : bullets) {
                if (!bullet.isFromPlayer() && bullet.isAlive()) {
                    bullet.kill();
                    break;
                }
            }
        }

        if (b->type == EntityType::TERRAIN) {
            if (a->type == EntityType::BULLET_PLAYER || a->type == EntityType::BULLET_BANDIT) {
                for (auto& bullet : bullets) {
                    if (bullet.isAlive()) {
                        bullet.kill();
                        break;
                    }
                }
            }
            if ((a->type == EntityType::PLAYER && b->type == EntityType::TERRAIN) ||
                (a->type == EntityType::TERRAIN && b->type == EntityType::PLAYER)) {
                            player->setOnGround(true);
                }
        }
    }
}

void Game::cleanupDead() {
    for (auto it = bullets.begin(); it != bullets.end(); ) {
        if (!it->isAlive()) {
            it->destroyBody(*world);
            it = bullets.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 135, 190, 230, 255); 
    SDL_RenderClear(renderer);

    renderBackgrounds();

    terrain->draw(renderer, cameraX);

    for (auto& fence : fences) {
        fence.draw(renderer, cameraX);
    }
    for (auto& cow : cows) {
        cow.draw(renderer, cameraX);
    }

    for (auto& bandit : bandits) {
        bandit.draw(renderer, cameraX);
    }

    for (auto& bullet : bullets) {
        bullet.draw(renderer, cameraX);
    }

    player->draw(renderer, cameraX);

    renderHUD();

    SDL_RenderPresent(renderer);
}

void Game::renderBackgrounds() {
    int startZone = static_cast<int>(std::floor(static_cast<float>(cameraX) / SCREEN_W));

    for (int z = startZone; z <= startZone + 2; ++z) {
        int screenX = z * SCREEN_W - cameraX;
        if (screenX + SCREEN_W < 0 || screenX > SCREEN_W) continue;

        int bgIndex = ((z % NUM_ZONES) + NUM_ZONES) % NUM_ZONES;
        SDL_Rect dst = {screenX, 0, SCREEN_W, SCREEN_H};

        if (bgs[bgIndex]) {
            SDL_RenderCopy(renderer, bgs[bgIndex], nullptr, &dst);
        }
    }
}

void Game::renderHUD() {
    SDL_SetRenderDrawColor(renderer, 220, 30, 30, 255);
    for (int i = 0; i < player->getHealth(); ++i) {
        SDL_Rect heart = {20 + i * 30, 20, 24, 24};
        SDL_RenderFillRect(renderer, &heart);
    }

    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
    int scoreBarW = std::min(score, 500);
    SDL_Rect scoreBar = {20, 55, scoreBarW, 10};
    SDL_RenderFillRect(renderer, &scoreBar);
}

void Game::cleanup() {
    for (auto& b : bullets)  b.destroyBody(*world);
    for (auto& b : bandits)  b.destroyBody(*world);
    for (auto& c : cows)     c.destroyBody(*world);
    for (auto& f : fences)   f.destroyBody(*world);

    bullets.clear();
    bandits.clear();
    cows.clear();
    fences.clear();

    delete player;
    player = nullptr;

    delete terrain;
    terrain = nullptr;

    delete world;
    world = nullptr;

    for (int i = 0; i < NUM_ZONES; ++i) {
        if (bgs[i]) {
            SDL_DestroyTexture(bgs[i]);
            bgs[i] = nullptr;
        }
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    IMG_Quit();
    SDL_Quit();
}