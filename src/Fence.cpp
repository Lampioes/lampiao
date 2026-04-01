#include "../include/Fence.h"
#include "../include/ContactListener.h"

Fence::Fence(b2World& world, float x, float y, float width, float height)
    : posX(x), posY(y), w(width), h(height)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(x / P2M, y / P2M);
    body = world.CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox(width / (2.0f * P2M), height / (2.0f * P2M));

    b2FixtureDef fixDef;
    fixDef.shape = &shape;
    fixDef.friction = 0.5f;

    EntityData* data = new EntityData{EntityType::FENCE, 0};
    fixDef.userData.pointer = reinterpret_cast<uintptr_t>(data);

    body->CreateFixture(&fixDef);
}

void Fence::takeDamage(int dmg) {
    health -= dmg;
    if (health <= 0) {
        destroyed = true;
    }
}

void Fence::draw(SDL_Renderer* renderer, int cameraX) {
    if (destroyed) return;

    int x = static_cast<int>(posX) - cameraX - static_cast<int>(w / 2);
    int y = static_cast<int>(posY) - static_cast<int>(h / 2);

    int r = 139, g = 90 + (health * 10), b = 43;
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    int postWidth = 8;
    int numPosts = 5;
    float spacing = w / (numPosts - 1);
    for (int i = 0; i < numPosts; ++i) {
        int px = x + static_cast<int>(i * spacing);
        SDL_Rect post = {px - postWidth / 2, y, postWidth, static_cast<int>(h)};
        SDL_RenderFillRect(renderer, &post);
    }

    SDL_SetRenderDrawColor(renderer, 160, 110, 60, 255);
    int numRails = 3;
    float railSpacing = h / (numRails + 1);
    for (int i = 1; i <= numRails; ++i) {
        int ry = y + static_cast<int>(i * railSpacing);
        SDL_Rect rail = {x, ry - 3, static_cast<int>(w), 6};
        SDL_RenderFillRect(renderer, &rail);
    }

    if (health < 3) {
        SDL_SetRenderDrawColor(renderer, 80, 50, 20, 255);
        SDL_RenderDrawLine(renderer, x + 20, y, x + 40, y + static_cast<int>(h));
    }
    if (health < 2) {
        SDL_RenderDrawLine(renderer, x + static_cast<int>(w) - 30, y,
                           x + static_cast<int>(w) - 50, y + static_cast<int>(h));
    }
}

void Fence::destroyBody(b2World& world) {
    if (body) {
        b2Fixture* f = body->GetFixtureList();
        while (f) {
            auto* data = reinterpret_cast<EntityData*>(f->GetUserData().pointer);
            delete data;
            f = f->GetNext();
        }
        world.DestroyBody(body);
        body = nullptr;
    }
}