#include "../include/Game.h"

#include <cmath>
#include <SDL2/SDL_image.h>

int Game::run()
{
    if (!init())
    {
        cleanup();
        return -1;
    }

    while (running)
    {
        handleEvents();

        world->Step(1.0f / 60.0f, 6, 2);
        player->update();
        updateCamera();

        render();
    }

    cleanup();
    return 0;
}

bool Game::init()
{
    return initSDL() && initAudio() && initPhysics() && loadAssets();
}

bool Game::initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        SDL_Log("Erro ao iniciar SDL: %s", SDL_GetError());
        return false;
    }

    const int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags)
    {
        SDL_Log("Erro ao iniciar SDL_image: %s", IMG_GetError());
        return false;
    }

    window = SDL_CreateWindow(
        "Lampioes 2D",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_W,
        SCREEN_H,
        0);

    if (!window)
    {
        SDL_Log("Erro ao criar janela: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        SDL_Log("Erro ao criar renderer: %s", SDL_GetError());
        return false;
    }

    return true;
}

bool Game::initAudio()
{
    if ((Mix_Init(MIX_INIT_OGG) & MIX_INIT_OGG) != MIX_INIT_OGG)
    {
        SDL_Log("Erro ao iniciar SDL_mixer: %s", Mix_GetError());
        return false;
    }

    if (Mix_OpenAudio(48000, AUDIO_S16SYS, 2, 2048) < 0)
    {
        SDL_Log("Erro ao abrir audio: %s", Mix_GetError());
        return false;
    }

    Mix_AllocateChannels(16);
    return true;
}

bool Game::initPhysics()
{
    world = new b2World(b2Vec2(0.0f, 0.125f));

    const float avgFloorY = 570.0f;

    b2BodyDef def;
    def.position.Set(0.0f, avgFloorY / P2M);
    b2Body *g = world->CreateBody(&def);

    b2EdgeShape edge;
    edge.SetTwoSided(b2Vec2(-10000.0f, 0.0f), b2Vec2(10000.0f, 0.0f));
    g->CreateFixture(&edge, 0.0f);

    return true;
}

bool Game::loadAssets()
{
    player = new Player(*world, renderer, 100, 100);

    bgs[0] = IMG_LoadTexture(renderer, "../sprites/montanhas.jpg");
    bgs[1] = IMG_LoadTexture(renderer, "../sprites/transicao-montanhas-deserto.jpg");
    bgs[2] = IMG_LoadTexture(renderer, "../sprites/calica-deserto.jpg");

    for (int i = 0; i < NUM_ZONES; ++i)
    {
        if (!bgs[i])
        {
            SDL_Log("Erro ao carregar background %d: %s", i, IMG_GetError());
            return false;
        }
    }

    bgMusic = Mix_LoadMUS("../audio/bg.ogg");
    if (!bgMusic)
    {
        SDL_Log("Erro ao carregar musica: %s", Mix_GetError());
        return false;
    }

    jumpSound = Mix_LoadWAV("../audio/jump.wav");
    if (!jumpSound)
    {
        SDL_Log("Erro ao carregar efeito de pulo: %s", Mix_GetError());
        return false;
    }

    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    Mix_VolumeChunk(jumpSound, MIX_MAX_VOLUME / 2);

    if (Mix_PlayMusic(bgMusic, -1) < 0)
    {
        SDL_Log("Erro ao tocar musica: %s", Mix_GetError());
        return false;
    }

    return true;
}

void Game::handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            running = false;

        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_RIGHT:
                player->moveRight();
                break;
            case SDLK_LEFT:
                player->moveLeft();
                break;
            case SDLK_SPACE:
                player->jump();
                Mix_PlayChannel(-1, jumpSound, 0);
                break;
            case SDLK_ESCAPE:
                running = false;
                break;
            }
        }
    }
}

void Game::updateCamera()
{
    int playerPixelX = static_cast<int>(player->getBody()->GetPosition().x * P2M);
    cameraX = playerPixelX - SCREEN_W / 2;
}

void Game::render()
{
    SDL_RenderClear(renderer);
    renderBackgrounds();
    renderGround();
    player->draw(renderer, cameraX);
    SDL_RenderPresent(renderer);
}

void Game::renderBackgrounds()
{
    int startZone = static_cast<int>(std::floor(static_cast<float>(cameraX) / SCREEN_W));

    for (int z = startZone; z <= startZone + 2; ++z)
    {
        int screenX = z * SCREEN_W - cameraX;
        if (screenX + SCREEN_W < 0 || screenX > SCREEN_W)
            continue;

        int bgIndex = ((z % NUM_ZONES) + NUM_ZONES) % NUM_ZONES;
        SDL_Rect dst = {screenX, 0, SCREEN_W, SCREEN_H};
        SDL_RenderCopy(renderer, bgs[bgIndex], nullptr, &dst);
    }
}

void Game::renderGround()
{
    int startZone = static_cast<int>(std::floor(static_cast<float>(cameraX) / SCREEN_W));

    for (int z = startZone; z <= startZone + 2; ++z)
    {
        int screenX = z * SCREEN_W - cameraX;
        if (screenX + SCREEN_W < 0 || screenX > SCREEN_W)
            continue;

        int bgIndex = ((z % NUM_ZONES) + NUM_ZONES) % NUM_ZONES;

        SDL_SetRenderDrawColor(renderer, 80, 50, 20, 255);
        SDL_Rect groundRect = {
            screenX,
            floorY[bgIndex],
            SCREEN_W,
            SCREEN_H - floorY[bgIndex]};
        SDL_RenderFillRect(renderer, &groundRect);
    }
}

void Game::cleanup()
{
    if (jumpSound)
    {
        Mix_FreeChunk(jumpSound);
        jumpSound = nullptr;
    }

    if (bgMusic)
    {
        Mix_FreeMusic(bgMusic);
        bgMusic = nullptr;
    }

    Mix_CloseAudio();
    Mix_Quit();

    for (int i = 0; i < NUM_ZONES; ++i)
    {
        if (bgs[i])
        {
            SDL_DestroyTexture(bgs[i]);
            bgs[i] = nullptr;
        }
    }

    delete player;
    player = nullptr;

    delete world;
    world = nullptr;

    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    IMG_Quit();
    SDL_Quit();
}
