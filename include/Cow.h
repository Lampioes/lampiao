#ifndef COW_H
#define COW_H

#include <SDL.h>

#include "StaticObject.h"

class Cow : public StaticObject {
public:
    Cow(b2World& world, float x, float y, int id);

    void draw(SDL_Renderer* renderer, int cameraX) override;
    int getId() const { return idVaca; }

private:
    int idVaca;

    static constexpr int LARGURA = 60;
    static constexpr int ALTURA = 40;
};

#endif
