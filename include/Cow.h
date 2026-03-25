#ifndef COW_H
#define COW_H

#include <SDL2/SDL.h>
#include <box2d/box2d.h>

class Cow {
public:
    Cow(b2World& world, float x, float y, int id);
    ~Cow() = default;

    void draw(SDL_Renderer* renderer, int cameraX);
    b2Body* getBody() const { return body; }
    int getId() const { return cowId; }

    void destroyBody(b2World& world);

private:
    b2Body* body = nullptr;
    int cowId;

    static constexpr float P2M = 30.0f;
    static constexpr int WIDTH = 60;
    static constexpr int HEIGHT = 40;
};

#endif