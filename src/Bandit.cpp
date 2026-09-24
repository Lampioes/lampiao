#include "../include/Bandit.h"
#include "../include/ContactListener.h"
#include <bit>
#include <cmath>

Bandit::Bandit(b2World& world, const Sprite& sprite, const b2Vec2& p, int id)
    : DynamicObject(world, p, DynamicBodyConfig{.fixedRotation = true}), idBandido(id),
      sprite(&sprite)
{
    b2PolygonShape forma;
    forma.SetAsBox(LARGURA / (2.0f * P2M), ALTURA / (2.0f * P2M));

    b2FixtureDef defFixacao;
    defFixacao.shape = &forma;
    defFixacao.density = 1.0f;
    defFixacao.friction = 0.3f;

    DadosEntidade* dados = new DadosEntidade{TipoEntidade::BANDIT, id};
    defFixacao.userData.pointer = std::bit_cast<uintptr_t>(dados);

    corpo->CreateFixture(&defFixacao);
}

void Bandit::update(float dt) {
    if (!vivo) return;
    corpo->SetLinearVelocity(b2Vec2(0, corpo->GetLinearVelocity().y));

    if (recarregando) {
        temporizadorRecarga += dt;
        if (temporizadorRecarga >= TEMPO_RECARGA) {
            recarregando = false;
            temporizadorRecarga = 0.0f;
            temporizadorTiro = 0.0f;
        }
    } else {
        temporizadorTiro += dt;
    }
}

bool Bandit::shouldShoot(float dt, float playerX) {
    if (!vivo || recarregando) return false;

    float meuX = corpo->GetPosition().x * P2M;
    if (std::abs(playerX - meuX) > ALCANCE_TIRO) return false;

    if (temporizadorTiro >= recargaTiro) {
        temporizadorTiro = 0.0f;
        recarregando = true;
        temporizadorRecarga = 0.0f;
        return true;
    }
    return false;
}

void Bandit::draw(SDL_Renderer* renderer, const Camera& camera) {
    if (!corpo) return;

    retanguloRender.x = static_cast<int>(corpo->GetPosition().x * P2M) - camera.x() - LARGURA / 2;
    retanguloRender.y = static_cast<int>(corpo->GetPosition().y * P2M) - camera.y() - ALTURA / 2;

    const int x = retanguloRender.x;
    const int y = retanguloRender.y;

    if (!vivo) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 80);
        SDL_RenderFillRect(renderer, &retanguloRender);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        return;
    }

    SDL_RendererFlip virar = viradoEsquerda ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    sprite->draw(renderer, retanguloRender, virar);

    float razaoVida = vida / 3.0f;
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_Rect barraFundo = {x, y - 20, LARGURA, 4};
    SDL_RenderFillRect(renderer, &barraFundo);
    SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
    SDL_Rect barraVida = {x, y - 20, (int)(LARGURA * razaoVida), 4};
    SDL_RenderFillRect(renderer, &barraVida);

    if (recarregando) {
        float razaoRecarga = temporizadorRecarga / TEMPO_RECARGA;
        SDL_SetRenderDrawColor(renderer, 50, 50, 200, 255);
        SDL_Rect barraRecarga = {x, y - 26, (int)(LARGURA * razaoRecarga), 3};
        SDL_RenderFillRect(renderer, &barraRecarga);
    }
}

float Bandit::getShootDirX(float playerX) {
    if (!corpo) return 1.0f;
    float meuX = corpo->GetPosition().x * P2M;
    viradoEsquerda = (playerX < meuX);
    return (playerX > meuX) ? 1.0f : -1.0f;
}

void Bandit::takeDamage() {
    vida--;
    if (vida <= 0) vivo = false;
}
