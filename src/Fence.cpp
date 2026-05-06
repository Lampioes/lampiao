#include "../include/Fence.h"
#include "../include/ContactListener.h"

Fence::Fence(b2World& world, float x, float y, float largura, float altura)
    : posX(x), posY(y), largura(largura), altura(altura)
{
    b2BodyDef defCorpo;
    defCorpo.type = b2_staticBody;
    defCorpo.position.Set(x / P2M, y / P2M);
    corpo = world.CreateBody(&defCorpo);

    b2PolygonShape forma;
    forma.SetAsBox(largura / (2.0f * P2M), altura / (2.0f * P2M));

    b2FixtureDef defFixacao;
    defFixacao.shape = &forma;
    defFixacao.friction = 0.5f;

    EntityData* dados = new EntityData{EntityType::FENCE, 0};
    defFixacao.userData.pointer = reinterpret_cast<uintptr_t>(dados);

    corpo->CreateFixture(&defFixacao);
}

void Fence::takeDamage(int dano) {
    vida -= dano;
    if (vida <= 0) {
        destruida = true;
    }
}

void Fence::draw(SDL_Renderer* renderer, int cameraX) {
    if (destruida) return;

    int x = static_cast<int>(posX) - cameraX - static_cast<int>(largura / 2);
    int y = static_cast<int>(posY) - static_cast<int>(altura / 2);

    int r = 139, g = 90 + (vida * 10), b = 43;
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    int larguraPoste = 8;
    int numPostes = 5;
    float espacamento = largura / (numPostes - 1);
    for (int i = 0; i < numPostes; ++i) {
        int px = x + static_cast<int>(i * espacamento);
        SDL_Rect poste = {px - larguraPoste / 2, y, larguraPoste, static_cast<int>(altura)};
        SDL_RenderFillRect(renderer, &poste);
    }

    SDL_SetRenderDrawColor(renderer, 160, 110, 60, 255);
    int numTrilhos = 3;
    float espacamentoTrilho = altura / (numTrilhos + 1);
    for (int i = 1; i <= numTrilhos; ++i) {
        int ry = y + static_cast<int>(i * espacamentoTrilho);
        SDL_Rect trilho = {x, ry - 3, static_cast<int>(largura), 6};
        SDL_RenderFillRect(renderer, &trilho);
    }

    if (vida < 3) {
        SDL_SetRenderDrawColor(renderer, 80, 50, 20, 255);
        SDL_RenderDrawLine(renderer, x + 20, y, x + 40, y + static_cast<int>(altura));
    }
    if (vida < 2) {
        SDL_RenderDrawLine(renderer, x + static_cast<int>(largura) - 30, y,
                           x + static_cast<int>(largura) - 50, y + static_cast<int>(altura));
    }
}

void Fence::destroyBody(b2World& world) {
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
