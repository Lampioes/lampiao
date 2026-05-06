#include "../include/Bullet.h"
#include "../include/ContactListener.h"
#include <SDL_image.h>
#include <cmath>

Bullet::Bullet(b2World& world, float x, float y, float dirX, float dirY, bool doJogador, SDL_Renderer* renderer)
    : doJogador(doJogador)
{
    float comprimento = std::sqrt(dirX * dirX + dirY * dirY);
    if (comprimento > 0.001f) {
        dirX /= comprimento;
        dirY /= comprimento;
    }

    b2BodyDef defCorpo;
    defCorpo.type = b2_dynamicBody;
    defCorpo.position.Set(x / P2M, y / P2M);
    defCorpo.bullet = true;
    defCorpo.gravityScale = 0.0f;
    corpo = world.CreateBody(&defCorpo);

    b2CircleShape forma;
    forma.m_radius = TAMANHO / (2.0f * P2M);

    b2FixtureDef defFixacao;
    defFixacao.shape = &forma;
    defFixacao.density = 0.1f;
    defFixacao.isSensor = true;

    EntityType tipo = doJogador ? EntityType::BULLET_PLAYER : EntityType::BULLET_BANDIT;
    EntityData* dados = new EntityData{tipo, 0};
    defFixacao.userData.pointer = reinterpret_cast<uintptr_t>(dados);

    corpo->CreateFixture(&defFixacao);

    corpo->SetLinearVelocity(b2Vec2(dirX * VELOCIDADE, dirY * VELOCIDADE));

    textura = IMG_LoadTexture(renderer, "../sprites/bullet.png");
}

void Bullet::update(float dt) {
    if (!viva) return;
    tempoVida += dt;
    if (tempoVida >= TEMPO_MAX_VIDA) {
        viva = false;
    }
}

void Bullet::draw(SDL_Renderer* renderer, int cameraX) {
    if (!viva || !corpo) return;

    int x = static_cast<int>(corpo->GetPosition().x * P2M) - cameraX;
    int y = static_cast<int>(corpo->GetPosition().y * P2M);

    if (textura) {
        SDL_Rect retanguloRender = {x - TAMANHO / 2, y - TAMANHO / 2, TAMANHO, TAMANHO};
        SDL_RenderCopy(renderer, textura, nullptr, &retanguloRender);
    } else {
        if (doJogador) {
            SDL_SetRenderDrawColor(renderer, 255, 200, 50, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);
        }
        SDL_Rect retangulo = {x - TAMANHO / 2, y - TAMANHO / 2, TAMANHO, TAMANHO};
        SDL_RenderFillRect(renderer, &retangulo);
    }
}

void Bullet::destroyBody(b2World& world) {
    if (corpo) {
        b2Fixture* f = corpo->GetFixtureList();
        while (f) {
            auto ptr = f->GetUserData().pointer;
            if (ptr) {
                delete reinterpret_cast<EntityData*>(ptr);
            }
            f = f->GetNext();
        }
        world.DestroyBody(corpo);
        corpo = nullptr;
    }
}
