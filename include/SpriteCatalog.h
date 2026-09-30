#ifndef SPRITE_CATALOG_H
#define SPRITE_CATALOG_H

#include <array>

#include <SDL.h>

#include "Sprite.h"

enum class SpriteId {
    VACA,
    BALA,
    BANDIDO,
    CORRER_1,
    CORRER_2,
    CORRER_3,
    CHAPEU,
    FUNDO_MONTANHAS,
    FUNDO_TRANSICAO,
    FUNDO_CALICA,
    FUNDO_CIDADE,
    TOTAL
};

class SpriteCatalog {
public:
    static constexpr int TOTAL_SPRITES = static_cast<int>(SpriteId::TOTAL);

    void loadAll(SDL_Renderer* renderer);
    void clear();

    const Sprite& get(SpriteId id) const { return sprites[static_cast<int>(id)]; }

private:
    std::array<Sprite, TOTAL_SPRITES> sprites{};
};

#endif
