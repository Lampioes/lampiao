#include "../include/Player.h"
#include "../include/ContactListener.h"
#include <SDL2/SDL_image.h>
#include <cmath>

Player::Player(b2World& world, SDL_Renderer* renderer, float x, float y) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x / P2M, y / P2M);
    bodyDef.fixedRotation = true;
    body = world.CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox(1.0f, 1.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;

    EntityData* data = new EntityData{EntityType::PLAYER, 0};
    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(data);

    body->CreateFixture(&fixtureDef);

    textures.push_back(IMG_LoadTexture(renderer, "../sprites/run-1.png"));
    textures.push_back(IMG_LoadTexture(renderer, "../sprites/run-2-e-parado.png"));
    textures.push_back(IMG_LoadTexture(renderer, "../sprites/run-3.png"));
}

void Player::moveRight() {
    if (isMoving) return;
    float currentX = body->GetPosition().x;
    targetX = std::min(currentX + MOVE_DISTANCE, MAX_X);
    isMoving = true;
    facingLeft = false;
}

void Player::moveLeft() {
    if (isMoving) return;
    float currentX = body->GetPosition().x;
    targetX = std::max(currentX - MOVE_DISTANCE, MIN_X);
    isMoving = true;
    facingLeft = true;
}

void Player::jump() {
    body->ApplyLinearImpulseToCenter(b2Vec2(0, JUMP_IMPULSE), true);
}

void Player::takeDamage() {
    health--;
}

void Player::clampPosition() {
    b2Vec2 pos = body->GetPosition();
    if (pos.x < MIN_X) {
        body->SetTransform(b2Vec2(MIN_X, pos.y), 0);
        body->SetLinearVelocity(b2Vec2(0, body->GetLinearVelocity().y));
        isMoving = false;
    } else if (pos.x > MAX_X) {
        body->SetTransform(b2Vec2(MAX_X, pos.y), 0);
        body->SetLinearVelocity(b2Vec2(0, body->GetLinearVelocity().y));
        isMoving = false;
    }
}

void Player::update(float dt) {
    
    if (shootCooldown > 0.0f) {
        shootCooldown -= dt;
    }

    if (isMoving) {
        float currentX = body->GetPosition().x;
        float diff = targetX - currentX;

        if (std::abs(diff) < 0.15f) {
            body->SetTransform(b2Vec2(targetX, body->GetPosition().y), 0);
            body->SetLinearVelocity(b2Vec2(0, body->GetLinearVelocity().y));
            isMoving = false;
            stuckFrames = 0;
        } else {
            // Detecta se está travado (colidindo com cerca/parede)
            if (std::abs(currentX - lastX) < 0.001f) {
                stuckFrames++;
                if (stuckFrames > 3) {
                    isMoving = false;
                    stuckFrames = 0;
                    body->SetLinearVelocity(b2Vec2(0, body->GetLinearVelocity().y));
                }
            } else {
                stuckFrames = 0;
            }

            if (isMoving) {
                float dir = (diff > 0) ? 1.0f : -1.0f;
                body->SetLinearVelocity(b2Vec2(dir * moveSpeed, body->GetLinearVelocity().y));
            }
        }
        lastX = currentX;
    }

    clampPosition();

    b2Vec2 velocity = body->GetLinearVelocity();
    if (std::abs(velocity.x) > 0.1f) {
        currentFrame = (SDL_GetTicks() / 500) % 3;
    } else {
        currentFrame = 1;
    }
}

void Player::draw(SDL_Renderer* renderer, int cameraX) {
    renderRect.x = (int)(body->GetPosition().x * P2M) - 75 - cameraX;
    renderRect.y = (int)(body->GetPosition().y * P2M) - 75;
    renderRect.w = 150;
    renderRect.h = 150;

    SDL_RendererFlip flip = facingLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    
    if (!textures.empty() && textures[currentFrame]) {
        SDL_RenderCopyEx(renderer, textures[currentFrame], NULL, &renderRect, 0.0, NULL, flip);
    } else {
        
        SDL_SetRenderDrawColor(renderer, 50, 130, 50, 255);
        SDL_Rect body_rect = {renderRect.x + 30, renderRect.y + 20, 90, 110};
        SDL_RenderFillRect(renderer, &body_rect);

        
        SDL_SetRenderDrawColor(renderer, 139, 90, 43, 255);
        SDL_Rect hat = {renderRect.x + 20, renderRect.y, 110, 25};
        SDL_RenderFillRect(renderer, &hat);
    }

    
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < health; ++i) {
        SDL_Rect heart = {renderRect.x + i * 14, renderRect.y - 20, 12, 12};
        SDL_RenderFillRect(renderer, &heart);
    }
}