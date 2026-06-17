#include "../include/Fence.h"
#include "../include/ContactListener.h"
#include <bit>

Fence::Fence(b2World& world, const b2Vec2& p, float largura, float altura)
    : StaticObject(world, p), largura(largura), altura(altura)
{
    b2PolygonShape forma;
    forma.SetAsBox(largura / (2.0f * P2M), altura / (2.0f * P2M));

    b2FixtureDef defFixacao;
    defFixacao.shape = &forma;
    defFixacao.friction = 0.5f;

    DadosEntidade* entidade = new DadosEntidade{TipoEntidade::FENCE, 0};
    defFixacao.userData.pointer = std::bit_cast<uintptr_t>(entidade);

    corpo->CreateFixture(&defFixacao);
}

void Fence::takeDamage(int dano) {
    vida -= dano;
    if (vida <= 0) destruida = true;
}

void Fence::draw(SDL_Renderer* renderer, int cameraX) {
    if (destruida) return;

    int x = (int)(posicao.x) - cameraX - (int)(largura / 2);
    int y = (int)(posicao.y) - (int)(altura / 2);

    int r = 139, g = 90 + (vida * 10), b = 43;
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    int larguraPoste = 8;
    int numPostes = 5;
    float espacamento = largura / (numPostes - 1);
    for (int i = 0; i < numPostes; ++i) {
        int px = x + (int)(i * espacamento);
        SDL_Rect poste = {px - larguraPoste / 2, y, larguraPoste, (int)(altura)};
        SDL_RenderFillRect(renderer, &poste);
    }

    SDL_SetRenderDrawColor(renderer, 160, 110, 60, 255);
    int numTrilhos = 3;
    float espacamentoTrilho = altura / (numTrilhos + 1);
    for (int i = 1; i <= numTrilhos; ++i) {
        int ry = y + (int)(i * espacamentoTrilho);
        SDL_Rect trilho = {x, ry - 3, (int)(largura), 6};
        SDL_RenderFillRect(renderer, &trilho);
    }

    if (vida < 3) {
        SDL_SetRenderDrawColor(renderer, 80, 50, 20, 255);
        SDL_RenderDrawLine(renderer, x + 20, y, x + 40, y + (int)(altura));
    }
    if (vida < 2) {
        SDL_RenderDrawLine(renderer, x + (int)(largura) - 30, y, x + (int)(largura) - 50, y + (int)(altura));
    }
}
