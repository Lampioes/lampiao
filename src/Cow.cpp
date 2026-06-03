#include "../include/Cow.h"
#include "../include/ContactListener.h"
#include <bit>

Cow::Cow(b2World& world, float x, float y, int id)
    : StaticObject(world, x, y), idVaca(id)
{
    b2PolygonShape forma;
    forma.SetAsBox(LARGURA / (2.0f * P2M), ALTURA / (2.0f * P2M));

    b2FixtureDef defFixacao;
    defFixacao.shape = &forma;
    defFixacao.isSensor = true;

    DadosEntidade* vaca = new DadosEntidade{TipoEntidade::COW, id};
    defFixacao.userData.pointer = std::bit_cast<uintptr_t>(vaca);

    corpo->CreateFixture(&defFixacao);
}

void Cow::draw(SDL_Renderer* renderer, int cameraX) {
    int x = static_cast<int>(corpo->GetPosition().x * P2M) - cameraX - LARGURA / 2;
    int y = static_cast<int>(corpo->GetPosition().y * P2M) - ALTURA / 2;

    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_Rect retanguloCorpo = {x, y, LARGURA, ALTURA};
    SDL_RenderFillRect(renderer, &retanguloCorpo);

    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_Rect mancha1 = {x + 10, y + 5, 15, 12};
    SDL_Rect mancha2 = {x + 35, y + 15, 12, 10};
    SDL_RenderFillRect(renderer, &mancha1);
    SDL_RenderFillRect(renderer, &mancha2);

    SDL_SetRenderDrawColor(renderer, 220, 200, 180, 255);
    SDL_Rect cabeca = {x + LARGURA, y + 5, 15, 20};
    SDL_RenderFillRect(renderer, &cabeca);

    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderDrawRect(renderer, &retanguloCorpo);
}
