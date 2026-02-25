#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <box2d/box2d.h>
#include <my-lib/macros.h>
#include <vector>

class Player {
    MYLIB_OO_ENCAPSULATE_PTR_READONLY(b2Body*, body)
    MYLIB_OO_ENCAPSULATE_SCALAR(int, currentFrame)

private:
    std::vector<SDL_Texture*> textures;
    SDL_Rect renderRect;

public:
    Player(b2World& world, SDL_Renderer* renderer, float x, float y);
    void update();
    void draw(SDL_Renderer* renderer);
};

#endif