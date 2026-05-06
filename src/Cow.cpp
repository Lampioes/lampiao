#include "../include/Cow.h"
#include "../include/ContactListener.h"

Cow::Cow(b2World& world, float x, float y, int id) : idVaca(id) {
    b2BodyDef defCorpo;
    defCorpo.type = b2_staticBody; // vaca fica parada
    defCorpo.position.Set(x / P2M, y / P2M);
    corpo = world.CreateBody(&defCorpo);

    b2PolygonShape forma;
    forma.SetAsBox(LARGURA / (2.0f * P2M), ALTURA / (2.0f * P2M));

    b2FixtureDef defFixacao;
    defFixacao.shape = &forma;

    EntityData* dados = new EntityData{EntityType::COW, id};
    defFixacao.userData.pointer = reinterpret_cast<uintptr_t>(dados);

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

void Cow::destroyBody(b2World& world) {
    if (corpo) {
        b2Fixture* f = corpo->GetFixtureList();
        while (f) {
            auto* dados = reinterpret_cast<EntityData*>(f->GetUserData().pointer);
            delete dados;
            f = f->GetNext();
        }
        world.DestroyBody(corpo);
        corpo = nullptr;
    }
}
