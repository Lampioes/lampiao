#ifndef SPRITE_H
#define SPRITE_H

#include <SDL.h>

class Sprite {
public:
    Sprite() = default;
    Sprite(SDL_Renderer* renderer, const char* path);
    ~Sprite();

    Sprite(const Sprite&) = delete;
    Sprite& operator=(const Sprite&) = delete;

    Sprite(Sprite&& other) noexcept;
    Sprite& operator=(Sprite&& other) noexcept;

    void load(SDL_Renderer* renderer, const char* path);

    void draw(SDL_Renderer* renderer, const SDL_Rect& dst, SDL_RendererFlip flip = SDL_FLIP_NONE) const;

    SDL_Texture* texture() const { return textura; }
    bool valid() const { return textura != nullptr; }

private:
    SDL_Texture* textura = nullptr;
};

#endif
