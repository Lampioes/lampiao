#include <SDL2/SDL.h>
#include <box2d/box2d.h>
#include "../include/Player.h"
#include <SDL2/SDL_image.h>

const float P2M = 30.0f;

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return -1;
    IMG_Init(IMG_INIT_JPG);

    SDL_Window* window = SDL_CreateWindow("Lampiões 2D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    b2Vec2 gravity(0.0f, 9.8f);
    b2World world(gravity);

    Player player(world, renderer, 100, 100);

    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(400.0f / P2M, 580.0f / P2M);
    b2Body* groundBody = world.CreateBody(&groundBodyDef);

    b2PolygonShape groundBox;
    groundBox.SetAsBox(400.0f / P2M, 10.0f / P2M);
    groundBody->CreateFixture(&groundBox, 0.0f);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            if (event.type == SDL_KEYDOWN) {
                b2Vec2 v = player.get_body()->GetLinearVelocity();
                switch (event.key.keysym.sym) {
                    case SDLK_RIGHT: player.get_body()->SetLinearVelocity(b2Vec2(5.0f, v.y)); break;
                    case SDLK_LEFT:  player.get_body()->SetLinearVelocity(b2Vec2(-5.0f, v.y)); break;
                    case SDLK_SPACE:
                        player.get_body()->ApplyLinearImpulseToCenter(b2Vec2(0, -10.0f), true);
                    break;
                }
            }
            if (event.type == SDL_KEYUP) {
                b2Vec2 v = player.get_body()->GetLinearVelocity();
                if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_LEFT)
                    player.get_body()->SetLinearVelocity(b2Vec2(0, v.y)); // Para de andar
            }
        }

        world.Step(1.0f / 60.0f, 6, 2);
        player.update();

        SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255);
        SDL_RenderClear(renderer);

        player.draw(renderer);

        SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
        SDL_Rect groundRect = { 0, 570, 800, 20 };
        SDL_RenderFillRect(renderer, &groundRect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}