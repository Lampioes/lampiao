#include "../include/Bandit.h"
#include "../include/ContactListener.h"
#include <bit>
#include <cmath>

Bandit::Bandit(b2World& world, SDL_Renderer* renderer, float x, float y, int id)
    : DynamicObject(world, x, y, DynamicBodyConfig{.fixedRotation = true}), idBandido(id)
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

    textura = IMG_LoadTexture(renderer, "../sprites/output_ixt5mp.gif");
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

void Bandit::draw(SDL_Renderer* renderer, int cameraX) {
    if (!corpo) return;

    int x = static_cast<int>(corpo->GetPosition().x * P2M) - cameraX - LARGURA / 2;
    int y = static_cast<int>(corpo->GetPosition().y * P2M) - ALTURA / 2;

    if (!vivo) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 80);
        SDL_Rect r = {x, y, LARGURA, ALTURA};
        SDL_RenderFillRect(renderer, &r);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        return;
    }

    SDL_Rect ondecolocar = {x, y, LARGURA, ALTURA};
    SDL_RendererFlip virar = viradoEsquerda ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    SDL_RenderCopyEx(renderer, textura, NULL, &ondecolocar, 0.0, NULL, virar);

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

