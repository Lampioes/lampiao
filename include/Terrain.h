#ifndef TERRAIN_H
#define TERRAIN_H

#include <SDL.h>
#include <box2d/box2d.h>
#include <array>
#include <vector>

class Terrain {
public:
    static constexpr int NUM_ZONES = 5;
    static constexpr int POINTS_PER_ZONE = 20;
    static constexpr int TOTAL_POINTS = NUM_ZONES * POINTS_PER_ZONE;
    static constexpr float P2M = 30.0f;
    static constexpr int SCREEN_W = 1920;
    static constexpr int SCREEN_H = 1080;

    Terrain(b2World& world);
    ~Terrain();

    void draw(SDL_Renderer* renderer, int cameraX);

    float getHeightAt(float pixelX) const;

    b2Body* getBody() const { return groundBody; }

private:
    b2Body* groundBody = nullptr;
    std::array<float, TOTAL_POINTS + 1> heights{};
    float pointSpacing;

    void generateHeightmap();
    void createPhysicsBody(b2World& world);
};

#endif