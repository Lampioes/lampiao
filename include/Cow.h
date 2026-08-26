#ifndef COW_H
#define COW_H

#include <SDL.h>

#include "Sprite.h"
#include "StaticObject.h"

class Cow : public StaticObject {
public:
    Cow(b2World& world, SDL_Renderer* renderer, const b2Vec2& p, int id);

    void draw(SDL_Renderer* renderer, int cameraX) override;
    int getId() const { return idVaca; }
    bool isAlive() const override { return viva; }
    void kill() { viva = false; }

private:
    int idVaca;
    bool viva = true;
    Sprite sprite;

    static constexpr int LARGURA = 120;
    static constexpr int ALTURA = 90;
};

#endif
