#include "../include/Player.h"
#include "../include/ContactListener.h"
#include <SDL_image.h>
#include <bit>
#include <cmath>

Player::Player(b2World& world, SDL_Renderer* renderer, float x, float y)
    : DynamicObject(world, x, y, DynamicBodyConfig{.fixedRotation = true})
{
    b2PolygonShape forma;
    forma.SetAsBox(1.0f, 1.0f);

    b2FixtureDef defFixacao;
    defFixacao.shape = &forma;
    defFixacao.density = 1.0f;
    defFixacao.friction = 0.3f;

    DadosEntidade* entidade = new DadosEntidade{TipoEntidade::PLAYER, 0};
    defFixacao.userData.pointer = std::bit_cast<uintptr_t>(entidade);

    corpo->CreateFixture(&defFixacao);

    texturas.push_back(IMG_LoadTexture(renderer, "../sprites/run-1.png"));
    texturas.push_back(IMG_LoadTexture(renderer, "../sprites/run-2-e-parado.png"));
    texturas.push_back(IMG_LoadTexture(renderer, "../sprites/run-3.png"));
}

void Player::moveRight() {
    if (!noChao) return; // bloqueia mudança de direção no ar
    if (emMovimento) return;
    float xAtual = corpo->GetPosition().x;
    alvoX = std::min(xAtual + DISTANCIA_MOVIMENTO, MAX_X);
    emMovimento = true;
    viradoEsquerda = false;
}

void Player::moveLeft() {
    if (!noChao) return; // bloqueia mudança de direção no ar
    if (emMovimento) return;
    float xAtual = corpo->GetPosition().x;
    alvoX = std::max(xAtual - DISTANCIA_MOVIMENTO, MIN_X);
    emMovimento = true;
    viradoEsquerda = true;
}

void Player::jump() {
    if (!noChao) return;
    corpo->ApplyLinearImpulseToCenter(b2Vec2(0, IMPULSO_PULO), true);
    noChao = false;
}

void Player::stop() {
    emMovimento = false;
    corpo->SetLinearVelocity(b2Vec2(0.0f, corpo->GetLinearVelocity().y));
}

void Player::takeDamage() {
    vida--;
}

void Player::clampPosition() {
    b2Vec2 posicao = corpo->GetPosition();
    if (posicao.x < MIN_X) {
        corpo->SetTransform(b2Vec2(MIN_X, posicao.y), 0);
        corpo->SetLinearVelocity(b2Vec2(0, corpo->GetLinearVelocity().y));
        emMovimento = false;
    } else if (posicao.x > MAX_X) {
        corpo->SetTransform(b2Vec2(MAX_X, posicao.y), 0);
        corpo->SetLinearVelocity(b2Vec2(0, corpo->GetLinearVelocity().y));
        emMovimento = false;
    }
}

void Player::update(float dt) {

    if (recargaTiro > 0.0f) {
        recargaTiro -= dt;
    }

    if (emMovimento) {
        float xAtual = corpo->GetPosition().x;
        float diferenca = alvoX - xAtual;

        if (std::abs(diferenca) < 0.15f) {
            corpo->SetTransform(b2Vec2(alvoX, corpo->GetPosition().y), 0);
            corpo->SetLinearVelocity(b2Vec2(0, corpo->GetLinearVelocity().y));
            emMovimento = false;
            framesPreso = 0;
        } else {
            if (std::abs(xAtual - ultimoX) < 0.001f) {
                framesPreso++;
                if (framesPreso > 3) {
                    emMovimento = false;
                    framesPreso = 0;
                    corpo->SetLinearVelocity(b2Vec2(0, corpo->GetLinearVelocity().y));
                }
            } else {
                framesPreso = 0;
            }

            float direcao = (diferenca > 0) ? 1.0f : -1.0f;
            corpo->SetLinearVelocity(b2Vec2(direcao * velocidadeMovimento, corpo->GetLinearVelocity().y));
        }
        ultimoX = xAtual;
    }

    clampPosition();

    b2Vec2 velocidade = corpo->GetLinearVelocity();
    if (std::abs(velocidade.x) > 0.1f) {
        frameAtual = (SDL_GetTicks() / 500) % 3;
    } else {
        frameAtual = 1;
    }
}

void Player::draw(SDL_Renderer* renderer, int cameraX) {
    retanguloRender.x = static_cast<int>(corpo->GetPosition().x * P2M) - 75 - cameraX;
    retanguloRender.y = static_cast<int>(corpo->GetPosition().y * P2M) - 75;
    retanguloRender.w = 150;
    retanguloRender.h = 150;

    SDL_RendererFlip flip = viradoEsquerda ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;


    if (!texturas.empty() && texturas[frameAtual]) {
        SDL_RenderCopyEx(renderer, texturas[frameAtual], NULL, &retanguloRender, 0.0, NULL, flip);
    } else {
        SDL_SetRenderDrawColor(renderer, 50, 130, 50, 255);
        SDL_Rect retanguloCorpo = {retanguloRender.x + 30, retanguloRender.y + 20, 90, 110};
        SDL_RenderFillRect(renderer, &retanguloCorpo);

        SDL_SetRenderDrawColor(renderer, 139, 90, 43, 255);
        SDL_Rect chapeu = {retanguloRender.x + 20, retanguloRender.y, 110, 25};
        SDL_RenderFillRect(renderer, &chapeu);
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < vida; ++i) {
        SDL_Rect coracao = {retanguloRender.x + i * 14, retanguloRender.y - 20, 12, 12};
        SDL_RenderFillRect(renderer, &coracao);
    }

}

void Player::captureCow() {

}

