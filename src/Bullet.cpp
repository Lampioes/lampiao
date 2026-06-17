#include "../include/Bullet.h"
#include "../include/ContactListener.h"
#include <bit>
#include <cmath>

Bullet::Bullet(b2World& world, const b2Vec2& p, const b2Vec2& dir, bool doJogador, SDL_Renderer* renderer)
    : DynamicObject(world, p, DynamicBodyConfig{.bullet = true, .gravityScale = 0.0f}),
      doJogador(doJogador),
      sprite(renderer, "../sprites/bullet.png")
{
    b2Vec2 direcao = dir;
    float comprimento = std::sqrt(direcao.x * direcao.x + direcao.y * direcao.y);
    if (comprimento > 0.001f) {
        direcao.x /= comprimento;
        direcao.y /= comprimento;
    }

    b2CircleShape forma;
    forma.m_radius = TAMANHO / (2.0f * P2M);

    b2FixtureDef defFixacao;
    defFixacao.shape = &forma;
    defFixacao.density = 0.1f;
    defFixacao.isSensor = true;

    TipoEntidade tipo = doJogador ? TipoEntidade::BULLET_PLAYER : TipoEntidade::BULLET_BANDIT;
    DadosEntidade* dados = new DadosEntidade{tipo, 0};
    defFixacao.userData.pointer = std::bit_cast<uintptr_t>(dados);

    corpo->CreateFixture(&defFixacao);

    corpo->SetLinearVelocity(b2Vec2(direcao.x * VELOCIDADE, direcao.y * VELOCIDADE));
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

    if (sprite.valid()) {
        SDL_Rect retanguloRender = {x - TAMANHO / 2, y - TAMANHO / 2, TAMANHO, TAMANHO};
        b2Vec2 vel = corpo->GetLinearVelocity();
        SDL_RendererFlip flip = vel.x < 0.0f ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        sprite.draw(renderer, retanguloRender, flip);
    } else {
        if (doJogador) SDL_SetRenderDrawColor(renderer, 255, 200, 50, 255);
        else SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);
        SDL_Rect retangulo = {x - TAMANHO / 2, y - TAMANHO / 2, TAMANHO, TAMANHO};
        SDL_RenderFillRect(renderer, &retangulo);
    }
}
