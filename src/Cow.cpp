#include "../include/Cow.h"
#include "../include/ContactListener.h"
#include <bit>

Cow::Cow(b2World& world, SDL_Renderer* renderer, const b2Vec2& p, int id)
    : StaticObject(world, p), idVaca(id),
      sprite(renderer, "../sprites/vaca.png")
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
    if (!viva) return;

    int x = static_cast<int>(corpo->GetPosition().x * P2M) - cameraX - LARGURA / 2;
    int y = static_cast<int>(corpo->GetPosition().y * P2M) - ALTURA / 2;

    //todo: fazer isso tbm no construtor da vaca (e nos lugar que usam draw)
    SDL_Rect ondecolocar = {x, y, LARGURA, ALTURA};
    sprite.draw(renderer, ondecolocar);
}
