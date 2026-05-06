#include "../include/Bandit.h"
#include "../include/ContactListener.h"
#include <cmath>

Bandit::Bandit(b2World& world, SDL_Renderer* renderer, float x, float y, int id) : banditId(id) {
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

    texture = IMG_LoadTexture(renderer, "../sprites/output_ixt5mp.gif");
}

void Bandit::update(float dt) {
    if (!alive) return;
    body->SetLinearVelocity(b2Vec2(0, body->GetLinearVelocity().y));

    if (reloading) {
        reloadTimer += dt;
        if (reloadTimer >= RELOAD_TIME) {
            reloading = false;
            reloadTimer = 0.0f;
            shootTimer = 0.0f;
        }
    } else {
        shootTimer += dt;
    }
}

bool Bandit::shouldShoot(float dt, float playerX) {
    if (!alive || reloading) return false;

    float myX = body->GetPosition().x * P2M;
    if (std::abs(playerX - myX) > SHOOT_RANGE) return false;

    if (shootTimer >= shootCooldown) {
        shootTimer = 0.0f;
        reloading = true;
        reloadTimer = 0.0f;
        return true;
    }
    return false;
}

void Bandit::draw(SDL_Renderer* renderer, int cameraX) {
    if (!body) return;

    int x = static_cast<int>(body->GetPosition().x * P2M) - cameraX - WIDTH / 2;
    int y = static_cast<int>(body->GetPosition().y * P2M) - HEIGHT / 2;

    if (!alive) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 80);
        SDL_Rect r = {x, y, WIDTH, HEIGHT};
        SDL_RenderFillRect(renderer, &r);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        return;
    }

    SDL_Rect dst = {x, y, WIDTH, HEIGHT};
    SDL_RendererFlip flip = facingLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    if (texture) {
        SDL_RenderCopyEx(renderer, texture, NULL, &dst, 0.0, NULL, flip);
    } else {
        SDL_SetRenderDrawColor(renderer, 139, 0, 0, 255);
        SDL_RenderFillRect(renderer, &dst);
    }

    float hpRatio = health / 3.0f;
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_Rect bgBar = {x, y - 20, WIDTH, 4};
    SDL_RenderFillRect(renderer, &bgBar);
    SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
    SDL_Rect hpBar = {x, y - 20, static_cast<int>(WIDTH * hpRatio), 4};
    SDL_RenderFillRect(renderer, &hpBar);

    if (reloading) {
        float reloadRatio = reloadTimer / RELOAD_TIME;
        SDL_SetRenderDrawColor(renderer, 50, 50, 200, 255);
        SDL_Rect reloadBar = {x, y - 26, static_cast<int>(WIDTH * reloadRatio), 3};
        SDL_RenderFillRect(renderer, &reloadBar);
    }
}

float Bandit::getShootDirX(float playerX) {
    if (!body) return 1.0f;
    float myX = body->GetPosition().x * P2M;
    facingLeft = (playerX < myX);
    return (playerX > myX) ? 1.0f : -1.0f;
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
