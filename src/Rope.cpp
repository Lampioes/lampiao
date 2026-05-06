#include "../include/Rope.h"
#include "../include/ContactListener.h"
#include <cmath>

void Rope::launch(b2World& world, float startX, float startY, float dirX, float dirY) {
    if (ativa) return;

    // Normaliza direção
    float comprimento = std::sqrt(dirX * dirX + dirY * dirY);
    if (comprimento < 0.001f) return;
    dirX /= comprimento;
    dirY /= comprimento;

    ativa = true;
    presa = false;
    tempoVida = 0.0f;

    b2Body* corpoAnterior = nullptr;

    for (int i = 0; i < NUM_SEGMENTOS; ++i) {
        RopeSegment seg;

        float sx = startX / 30.0f + dirX * i * COMPRIMENTO_SEGMENTO;
        float sy = startY / 30.0f + dirY * i * COMPRIMENTO_SEGMENTO;

        b2BodyDef defCorpo;
        defCorpo.type = b2_dynamicBody;
        defCorpo.position.Set(sx, sy);
        defCorpo.linearDamping = 0.5f;
        seg.corpo = world.CreateBody(&defCorpo);

        b2CircleShape forma;
        forma.m_radius = 0.05f;

        b2FixtureDef defFixacao;
        defFixacao.shape = &forma;
        defFixacao.density = 0.5f;
        defFixacao.friction = 0.3f;

        // Só o último segmento (ponta) é sensor para detectar vaca
        if (i == NUM_SEGMENTOS - 1) {
            defFixacao.isSensor = true;
            EntityData* dados = new EntityData{EntityType::ROPE_TIP, 0};
            defFixacao.userData.pointer = reinterpret_cast<uintptr_t>(dados);
        }

        seg.corpo->CreateFixture(&defFixacao);

        // Dá velocidade inicial na direção do lançamento
        seg.corpo->SetLinearVelocity(b2Vec2(dirX * VELOCIDADE_LANCAMENTO, dirY * VELOCIDADE_LANCAMENTO));

        // Cria juncao com o segmento anterior
        if (corpoAnterior) {
            b2RevoluteJointDef defJuncao;
            defJuncao.Initialize(corpoAnterior, seg.corpo,
                                b2Vec2(sx - dirX * COMPRIMENTO_SEGMENTO * 0.5f,
                                        sy - dirY * COMPRIMENTO_SEGMENTO * 0.5f));
            defJuncao.collideConnected = false;
            seg.juncao = world.CreateJoint(&defJuncao);
        }

        segmentos.push_back(seg);
        corpoAnterior = seg.corpo;
    }
}

void Rope::retract(b2World& world) {
    for (auto& seg : segmentos) {
        if (seg.juncao) {
            world.DestroyJoint(seg.juncao);
            seg.juncao = nullptr;
        }
        if (seg.corpo) {
            // Limpa userData se existir
            b2Fixture* f = seg.corpo->GetFixtureList();
            while (f) {
                auto ptr = f->GetUserData().pointer;
                if (ptr) {
                    delete reinterpret_cast<EntityData*>(ptr);
                }
                f = f->GetNext();
            }
            world.DestroyBody(seg.corpo);
            seg.corpo = nullptr;
        }
    }
    segmentos.clear();
    ativa = false;
    presa = false;
}

void Rope::update(float dt) {
    if (!ativa) return;

    tempoVida += dt;
    if (tempoVida >= TEMPO_MAX_VIDA && !presa) {
        // Será retraída pelo Game no próximo frame
        ativa = false;
    }
}

void Rope::draw(SDL_Renderer* renderer, int cameraX) {
    if (!ativa || segmentos.empty()) return;

    SDL_SetRenderDrawColor(renderer, 180, 140, 80, 255); // cor de corda

    b2Body* anterior = nullptr;
    for (auto& seg : segmentos) {
        if (!seg.corpo) continue;
        if (anterior) {
            int x1 = static_cast<int>(anterior->GetPosition().x * 30.0f) - cameraX;
            int y1 = static_cast<int>(anterior->GetPosition().y * 30.0f);
            int x2 = static_cast<int>(seg.corpo->GetPosition().x * 30.0f) - cameraX;
            int y2 = static_cast<int>(seg.corpo->GetPosition().y * 30.0f);

            // Desenha linha grossa (3 linhas paralelas)
            for (int deslocamento = -1; deslocamento <= 1; ++deslocamento) {
                SDL_RenderDrawLine(renderer, x1, y1 + deslocamento, x2, y2 + deslocamento);
            }
        }
        anterior = seg.corpo;
    }

    // Desenha a ponta como um laço (círculo)
    if (!segmentos.empty()) {
        auto& ponta = segmentos.back();
        if (ponta.corpo) {
            int tx = static_cast<int>(ponta.corpo->GetPosition().x * 30.0f) - cameraX;
            int ty = static_cast<int>(ponta.corpo->GetPosition().y * 30.0f);
            SDL_SetRenderDrawColor(renderer, 200, 160, 90, 255);
            SDL_Rect laco = {tx - 8, ty - 8, 16, 16};
            SDL_RenderDrawRect(renderer, &laco);
        }
    }
}

b2Body* Rope::getTipBody() const {
    if (segmentos.empty()) return nullptr;
    return segmentos.back().corpo;
}

void Rope::attachTo(b2World& world, b2Body* alvo) {
    if (segmentos.empty() || !alvo) return;

    b2Body* corpoPonta = segmentos.back().corpo;
    if (!corpoPonta) return;

    b2RevoluteJointDef defJuncao;
    defJuncao.Initialize(corpoPonta, alvo, alvo->GetPosition());
    defJuncao.collideConnected = false;
    world.CreateJoint(&defJuncao);

    presa = true;
}
