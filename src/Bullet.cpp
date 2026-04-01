#include "../include/Bullet.h"
#include "../include/ContactListener.h"
#include <SDL_image.h>
#include <cmath>

Bullet::Bullet(b2World& world, float x, float y, float dirX, float dirY, bool fromPlayer, SDL_Renderer* renderer)
    : fromPlayer(fromPlayer)
{
    float len = std::sqrt(dirX * dirX + dirY * dirY);
    if (len > 0.001f) {
        dirX /= len;
        dirY /= len;
    }

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x / P2M, y / P2M);
    bodyDef.bullet = true;
    bodyDef.gravityScale = 0.0f;
    body = world.CreateBody(&bodyDef);

    b2CircleShape shape;
    shape.m_radius = SIZE / (2.0f * P2M);

    b2FixtureDef fixDef;
    fixDef.shape = &shape;
    fixDef.density = 0.1f;
    fixDef.isSensor = true;

    EntityType type = fromPlayer ? EntityType::BULLET_PLAYER : EntityType::BULLET_BANDIT;
    EntityData* data = new EntityData{type, 0};
    fixDef.userData.pointer = reinterpret_cast<uintptr_t>(data);

    body->CreateFixture(&fixDef);

    body->SetLinearVelocity(b2Vec2(dirX * SPEED, dirY * SPEED));

    // Carrega o sprite
    texture = IMG_LoadTexture(renderer, "../sprites/bullet.png");
}

void Bullet::update(float dt) {
    if (!alive) return;
    lifetime += dt;
    if (lifetime >= MAX_LIFETIME) {
        alive = false;
    }
}

void Bullet::draw(SDL_Renderer* renderer, int cameraX) {
    if (!alive || !body) return;

    int x = static_cast<int>(body->GetPosition().x * P2M) - cameraX;
    int y = static_cast<int>(body->GetPosition().y * P2M);

    if (texture) {
        SDL_Rect renderRect = {x - SIZE / 2, y - SIZE / 2, SIZE, SIZE};
        SDL_RenderCopy(renderer, texture, nullptr, &renderRect);
    } else {
        // Fallback: desenha um retângulo colorido se a textura não carregar
        if (fromPlayer) {
            SDL_SetRenderDrawColor(renderer, 255, 200, 50, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);
        }
        SDL_Rect rect = {x - SIZE / 2, y - SIZE / 2, SIZE, SIZE};
        SDL_RenderFillRect(renderer, &rect);
    }
}

void Bullet::destroyBody(b2World& world) {
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
