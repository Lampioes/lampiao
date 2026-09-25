#ifndef COW_H
#define COW_H

#include <SDL.h>

#include "Sprite.h"
#include "StaticObject.h"

class Cow : public StaticObject {
public:
    Cow(b2World& world, const Sprite& sprite, const b2Vec2& p, int id);

    void draw(SDL_Renderer* renderer, const Camera& camera) override;
    int getId() const { return idVaca; }
    bool isAlive() const override { return viva; }
    void kill() { viva = false; }

private:
    static constexpr int LARGURA = 120;
    static constexpr int ALTURA = 90;

    int idVaca;
    bool viva = true;
    const Sprite* sprite;
    SDL_Rect retanguloRender{0, 0, LARGURA, ALTURA};
};

#endif
