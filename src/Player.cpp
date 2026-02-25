#include "../include/Player.h"
#include <SDL2/SDL_image.h>
#include <iostream>

Player::Player(b2World& world, SDL_Renderer* renderer, float x, float y) {
    this->set_currentFrame(1);

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x / 30.0f, y / 30.0f);
    bodyDef.fixedRotation = true;
    this->body = world.CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox(1.0f, 1.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    this->body->CreateFixture(&fixtureDef);

    textures.push_back(IMG_LoadTexture(renderer, "../sprites/run-1.jpg"));
    textures.push_back(IMG_LoadTexture(renderer, "../sprites/run-2-e-parado.jpg"));
    textures.push_back(IMG_LoadTexture(renderer, "../sprites/run-3.jpg"));

    SDL_Texture* tex = IMG_LoadTexture(renderer, "../sprites/run-1.jpg");
    if (!tex) {
        std::cout << "ERRO: Nao foi possivel carregar a imagem! SDL_Error: " << SDL_GetError() << std::endl;
    } else {
        textures.push_back(tex);
    }
}

void Player::update() {
    b2Vec2 velocity = this->body->GetLinearVelocity();
    if (std::abs(velocity.x) > 0.1f) this->set_currentFrame((SDL_GetTicks() / 100) % 3);
    else this->set_currentFrame(1);

}

void Player::draw(SDL_Renderer* renderer) {
    renderRect.x = (int)(this->body->GetPosition().x * 30.0f) - 32;
    renderRect.y = (int)(this->body->GetPosition().y * 30.0f) - 32;
    renderRect.w = 64;
    renderRect.h = 64;

    SDL_RenderCopy(renderer, textures[this->get_currentFrame()], NULL, &renderRect);
}