#include "../include/Sprite.h"

#include <SDL_image.h>

Sprite::Sprite(SDL_Renderer* renderer, const char* path) {
    load(renderer, path);
}

void Sprite::load(SDL_Renderer* renderer, const char* path) {
    textura.reset(IMG_LoadTexture(renderer, path));
}

void Sprite::draw(SDL_Renderer* renderer, const SDL_Rect& dst, SDL_RendererFlip flip) const {
    if (!textura) return;
    SDL_RenderCopyEx(renderer, textura.get(), nullptr, &dst, 0.0, nullptr, flip);
}
