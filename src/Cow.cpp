#include "../include/Cow.h"
#include "../include/ContactListener.h"

Cow::Cow(b2World& world, float x, float y, int id) : cowId(id) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody; // vaca fica parada
    bodyDef.position.Set(x / P2M, y / P2M);
    body = world.CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox(WIDTH / (2.0f * P2M), HEIGHT / (2.0f * P2M));

    b2FixtureDef fixDef;
    fixDef.shape = &shape;

    EntityData* data = new EntityData{EntityType::COW, id};
    fixDef.userData.pointer = reinterpret_cast<uintptr_t>(data);

    body->CreateFixture(&fixDef);
}

void Cow::draw(SDL_Renderer* renderer, int cameraX) {
    int x = static_cast<int>(body->GetPosition().x * P2M) - cameraX - WIDTH / 2;
    int y = static_cast<int>(body->GetPosition().y * P2M) - HEIGHT / 2;

    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_Rect bodyRect = {x, y, WIDTH, HEIGHT};
    SDL_RenderFillRect(renderer, &bodyRect);

    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_Rect spot1 = {x + 10, y + 5, 15, 12};
    SDL_Rect spot2 = {x + 35, y + 15, 12, 10};
    SDL_RenderFillRect(renderer, &spot1);
    SDL_RenderFillRect(renderer, &spot2);

    SDL_SetRenderDrawColor(renderer, 220, 200, 180, 255);
    SDL_Rect head = {x + WIDTH, y + 5, 15, 20};
    SDL_RenderFillRect(renderer, &head);

    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderDrawRect(renderer, &bodyRect);
}

void Cow::destroyBody(b2World& world) {
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