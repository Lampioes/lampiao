#include "../include/Cow.h"
#include "../include/ContactListener.h"
#include <bit>

Cow::Cow(b2World& world, const Sprite& sprite, const b2Vec2& p, int id)
    : StaticObject(world, p), idVaca(id), sprite(&sprite)
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

void Cow::draw(SDL_Renderer* renderer, const Camera& camera) {
    if (!viva) return;

    retanguloRender.x = static_cast<int>(corpo->GetPosition().x * P2M) - camera.x() - LARGURA / 2;
    retanguloRender.y = static_cast<int>(corpo->GetPosition().y * P2M) - camera.y() - ALTURA / 2;

    sprite->draw(renderer, retanguloRender);
}
