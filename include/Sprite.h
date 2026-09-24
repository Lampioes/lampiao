#ifndef SPRITE_H
#define SPRITE_H

#include <memory>

#include <SDL.h>

// deleter pro unique_ptr saber como destruir uma SDL_Texture
struct SDLTextureDeleter {
    void operator()(SDL_Texture* t) const noexcept { SDL_DestroyTexture(t); }
};

using TexturaPtr = std::unique_ptr<SDL_Texture, SDLTextureDeleter>;

// uma sprite por arquivo de sprite: quem carrega todas e a SpriteCatalog,
// e as classes ja recebem a sprite pronta (com a textura do sdl) no construtor
class Sprite {
public:
    Sprite() = default;
    Sprite(SDL_Renderer* renderer, const char* path);

    // sem destrutor: o unique_ptr libera a textura sozinho.
    // copia continua proibida (textura tem um dono só), e o move o compilador gera certo.
    Sprite(const Sprite&) = delete;
    Sprite& operator=(const Sprite&) = delete;
    Sprite(Sprite&&) noexcept = default;
    Sprite& operator=(Sprite&&) noexcept = default;

    void load(SDL_Renderer* renderer, const char* path);

    void draw(SDL_Renderer* renderer, const SDL_Rect& dst, SDL_RendererFlip flip = SDL_FLIP_NONE) const;

    SDL_Texture* texture() const { return textura.get(); }
    bool valid() const { return textura != nullptr; }

private:
    TexturaPtr textura;
    SDL_Rect retanguloMalucoClipagem;
};

#endif
