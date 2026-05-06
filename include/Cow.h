#ifndef COW_H
#define COW_H

#include <SDL.h>
#include <box2d/box2d.h>

class Cow {
public:
    Cow(b2World& world, float x, float y, int id);
    ~Cow() = default;

    void draw(SDL_Renderer* renderer, int cameraX);
    b2Body* getBody() const { return corpo; }
    int getId() const { return idVaca; }

    void destroyBody(b2World& world);

private:
    b2Body* corpo = nullptr;
    int idVaca;

    static constexpr float P2M = 30.0f;
    static constexpr int LARGURA = 60;
    static constexpr int ALTURA = 40;
};

#endif
