#ifndef ROPE_H
#define ROPE_H

#include <SDL.h>
#include <box2d/box2d.h>
#include <list>

struct RopeSegment {
    b2Body* body = nullptr;
    b2Joint* joint = nullptr; 
};

class Rope {
public:
    Rope() = default;
    ~Rope() = default;

    void launch(b2World& world, float startX, float startY, float dirX, float dirY);
    void retract(b2World& world);
    void update(float dt);
    void draw(SDL_Renderer* renderer, int cameraX);
    bool isActive() const { return active; }

    b2Body* getTipBody() const;

    void attachTo(b2World& world, b2Body* target);
    bool isAttached() const { return attached; }

private:
    std::list<RopeSegment> segments;
    bool active = false;
    bool attached = false;
    float lifetime = 0.0f;

    static constexpr float P2M = 30.0f;
    static constexpr int NUM_SEGMENTS = 8;
    static constexpr float SEGMENT_LENGTH = 0.5f; 
    static constexpr float MAX_LIFETIME = 2.0f;
    static constexpr float LAUNCH_SPEED = 10.0f;
};

#endif