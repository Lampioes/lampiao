#include <SDL2/SDL.h>
#include <box2d/box2d.h>
#include "../include/Player.h"
#include <SDL2/SDL_image.h>

const float P2M = 30.0f;
const int SCREEN_W = 1920;
const int SCREEN_H = 1080;
const int NUM_ZONES = 3;

const int FLOOR_Y[NUM_ZONES] = { 570, 570, 570 };

struct GameState {
    SDL_Window*   window   = nullptr;
    SDL_Renderer* renderer = nullptr;
    b2World*      world    = nullptr;
    Player*       player   = nullptr;
    SDL_Texture*  bgs[NUM_ZONES] = {};
    int           cameraX  = 0;
    bool          running  = true;
};

void initSDL(GameState& gs) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    gs.window   = SDL_CreateWindow("Lampiões 2D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H, 0);
    gs.renderer = SDL_CreateRenderer(gs.window, -1, SDL_RENDERER_ACCELERATED);
}

void initPhysics(GameState& gs) {
    gs.world = new b2World(b2Vec2(0.0f, 0.125f));

    float avgFloorY = 570.0f;

    b2BodyDef def;
    def.position.Set(0.0f, avgFloorY / P2M);
    b2Body* g = gs.world->CreateBody(&def);

    b2EdgeShape edge;
    edge.SetTwoSided(b2Vec2(-10000.0f, 0.0f), b2Vec2(10000.0f, 0.0f));
    g->CreateFixture(&edge, 0.0f);
}

void loadAssets(GameState& gs) {
    gs.player = new Player(*gs.world, gs.renderer, 100, 100);

    gs.bgs[0] = IMG_LoadTexture(gs.renderer, "../sprites/montanhas.jpg");
    gs.bgs[1] = IMG_LoadTexture(gs.renderer, "../sprites/transicao-montanhas-deserto.jpg");
    gs.bgs[2] = IMG_LoadTexture(gs.renderer, "../sprites/calica-deserto.jpg");
}

void handleEvents(GameState& gs) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) gs.running = false;
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_RIGHT: gs.player->moveRight(); break;
                case SDLK_LEFT:  gs.player->moveLeft();  break;
                case SDLK_SPACE: gs.player->jump();      break;
                case SDLK_ESCAPE: gs.running = false;    break;
            }
        }
    }
}

void updateCamera(GameState& gs) {
    int playerPixelX = (int)(gs.player->getBody()->GetPosition().x * P2M);
    gs.cameraX = playerPixelX - SCREEN_W / 2;
}

void renderBackgrounds(GameState& gs) {
    int startZone = (int)floor((float)gs.cameraX / SCREEN_W);
    for (int z = startZone; z <= startZone + 2; z++) {
        int screenX = z * SCREEN_W - gs.cameraX;
        if (screenX + SCREEN_W < 0 || screenX > SCREEN_W) continue;
        int bgIndex = ((z % NUM_ZONES) + NUM_ZONES) % NUM_ZONES;
        SDL_Rect dst = { screenX, 0, SCREEN_W, SCREEN_H };
        SDL_RenderCopy(gs.renderer, gs.bgs[bgIndex], NULL, &dst);
    }
}

void renderGround(GameState& gs) {
    int startZone = (int)floor((float)gs.cameraX / SCREEN_W);
    for (int z = startZone; z <= startZone + 2; z++) {
        int screenX = z * SCREEN_W - gs.cameraX;
        if (screenX + SCREEN_W < 0 || screenX > SCREEN_W) continue;
        int bgIndex = ((z % NUM_ZONES) + NUM_ZONES) % NUM_ZONES;
        SDL_SetRenderDrawColor(gs.renderer, 80, 50, 20, 255);
        SDL_Rect groundRect = { screenX, FLOOR_Y[bgIndex], SCREEN_W, SCREEN_H - FLOOR_Y[bgIndex] };
        SDL_RenderFillRect(gs.renderer, &groundRect);
    }
}

void cleanup(GameState& gs) {
    for (int i = 0; i < NUM_ZONES; i++) {
        if (gs.bgs[i]) SDL_DestroyTexture(gs.bgs[i]);
    }
    delete gs.player;
    delete gs.world;
    SDL_DestroyRenderer(gs.renderer);
    SDL_DestroyWindow(gs.window);
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char** argv) {
    GameState gs;
    initSDL(gs);
    initPhysics(gs);
    loadAssets(gs);

    while (gs.running) {
        handleEvents(gs);

        gs.world->Step(1.0f / 60.0f, 6, 2);
        gs.player->update();
        updateCamera(gs);

        SDL_RenderClear(gs.renderer);
        renderBackgrounds(gs);
        renderGround(gs);
        gs.player->draw(gs.renderer, gs.cameraX);
        SDL_RenderPresent(gs.renderer);
    }

    cleanup(gs);
    return 0;
}