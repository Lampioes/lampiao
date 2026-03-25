#include "../include/Bandit.h"
#include "../include/ContactListener.h"
#include <cmath>

Bandit::Bandit(b2World& world, float x, float y, int id) : banditId(id) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x / P2M, y / P2M);
    bodyDef.fixedRotation = true;
    body = world.CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox(WIDTH / (2.0f * P2M), HEIGHT / (2.0f * P2M));

    b2FixtureDef fixDef;
    fixDef.shape = &shape;
    fixDef.density = 1.0f;
    fixDef.friction = 0.3f;

    EntityData* data = new EntityData{EntityType::BANDIT, id};
    fixDef.userData.pointer = reinterpret_cast<uintptr_t>(data);

    body->CreateFixture(&fixDef);
}

void Bandit::update(float dt) {
    if (!alive) return;
    // Bandido fica parado esperando pra atirar (AI simples)
    // Gravidade do Box2D cuida de mantê-lo no chão
}

void Bandit::draw(SDL_Renderer* renderer, int cameraX) {
    if (!alive || !body) return;

    int x = static_cast<int>(body->GetPosition().x * P2M) - cameraX - WIDTH / 2;
    int y = static_cast<int>(body->GetPosition().y * P2M) - HEIGHT / 2;

    // Corpo (vermelho escuro)
    SDL_SetRenderDrawColor(renderer, 150, 40, 40, 255);
    SDL_Rect bodyRect = {x, y, WIDTH, HEIGHT};
    SDL_RenderFillRect(renderer, &bodyRect);

    // Chapéu (preto)
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_Rect hat = {x - 5, y - 10, WIDTH + 10, 12};
    SDL_RenderFillRect(renderer, &hat);

    // Barra de vida
    if (health > 0 && health < 3) {
        float hpRatio = static_cast<float>(health) / 3.0f;
        int barW = static_cast<int>(WIDTH * hpRatio);
        SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
        SDL_Rect hpBar = {x, y - 18, barW, 4};
        SDL_RenderFillRect(renderer, &hpBar);
    }
}

bool Bandit::shouldShoot(float dt) {
    if (!alive) return false;
    shootTimer += dt;
    if (shootTimer >= shootCooldown) {
        shootTimer = 0.0f;
        return true;
    }
    return false;
}

float Bandit::getShootDirX(float playerX) const {
    if (!body) return 1.0f;
    float bx = body->GetPosition().x * P2M;
    return (playerX > bx) ? 1.0f : -1.0f;
}

void Bandit::takeDamage() {
    health--;
    if (health <= 0) {
        alive = false;
    }
}

void Bandit::destroyBody(b2World& world) {
    if (body) {
        b2Fixture* f = body->GetFixtureList();
        while (f) {
            auto ptr = f->GetUserData().pointer;
            if (ptr) {
                delete reinterpret_cast<EntityData*>(ptr);
            }
            f = f->GetNext();
        }
        world.DestroyBody(body);
        body = nullptr;
    }
}
