#include "../include/Rope.h"
#include "../include/ContactListener.h"
#include <cmath>

void Rope::launch(b2World& world, float startX, float startY, float dirX, float dirY) {
    if (active) return;

    // Normaliza direção
    float len = std::sqrt(dirX * dirX + dirY * dirY);
    if (len < 0.001f) return;
    dirX /= len;
    dirY /= len;

    active = true;
    attached = false;
    lifetime = 0.0f;

    b2Body* prevBody = nullptr;

    for (int i = 0; i < NUM_SEGMENTS; ++i) {
        RopeSegment seg;

        float sx = startX / 30.0f + dirX * i * SEGMENT_LENGTH;
        float sy = startY / 30.0f + dirY * i * SEGMENT_LENGTH;

        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(sx, sy);
        bodyDef.linearDamping = 0.5f;
        seg.body = world.CreateBody(&bodyDef);

        b2CircleShape shape;
        shape.m_radius = 0.05f;

        b2FixtureDef fixDef;
        fixDef.shape = &shape;
        fixDef.density = 0.5f;
        fixDef.friction = 0.3f;

        // Só o último segmento (ponta) é sensor para detectar vaca
        if (i == NUM_SEGMENTS - 1) {
            fixDef.isSensor = true;
            EntityData* data = new EntityData{EntityType::ROPE_TIP, 0};
            fixDef.userData.pointer = reinterpret_cast<uintptr_t>(data);
        }

        seg.body->CreateFixture(&fixDef);

        // Dá velocidade inicial na direção do lançamento
        seg.body->SetLinearVelocity(b2Vec2(dirX * LAUNCH_SPEED, dirY * LAUNCH_SPEED));

        // Cria joint com o segmento anterior
        if (prevBody) {
            b2RevoluteJointDef jointDef;
            jointDef.Initialize(prevBody, seg.body,
                                b2Vec2(sx - dirX * SEGMENT_LENGTH * 0.5f,
                                        sy - dirY * SEGMENT_LENGTH * 0.5f));
            jointDef.collideConnected = false;
            seg.joint = world.CreateJoint(&jointDef);
        }

        segments.push_back(seg);
        prevBody = seg.body;
    }
}

void Rope::retract(b2World& world) {
    for (auto& seg : segments) {
        if (seg.joint) {
            world.DestroyJoint(seg.joint);
            seg.joint = nullptr;
        }
        if (seg.body) {
            // Limpa userData se existir
            b2Fixture* f = seg.body->GetFixtureList();
            while (f) {
                auto ptr = f->GetUserData().pointer;
                if (ptr) {
                    delete reinterpret_cast<EntityData*>(ptr);
                }
                f = f->GetNext();
            }
            world.DestroyBody(seg.body);
            seg.body = nullptr;
        }
    }
    segments.clear();
    active = false;
    attached = false;
}

void Rope::update(float dt) {
    if (!active) return;

    lifetime += dt;
    if (lifetime >= MAX_LIFETIME && !attached) {
        // Será retraída pelo Game no próximo frame
        active = false;
    }
}

void Rope::draw(SDL_Renderer* renderer, int cameraX) {
    if (!active || segments.empty()) return;

    SDL_SetRenderDrawColor(renderer, 180, 140, 80, 255); // cor de corda

    b2Body* prev = nullptr;
    for (auto& seg : segments) {
        if (!seg.body) continue;
        if (prev) {
            int x1 = static_cast<int>(prev->GetPosition().x * 30.0f) - cameraX;
            int y1 = static_cast<int>(prev->GetPosition().y * 30.0f);
            int x2 = static_cast<int>(seg.body->GetPosition().x * 30.0f) - cameraX;
            int y2 = static_cast<int>(seg.body->GetPosition().y * 30.0f);

            // Desenha linha grossa (3 linhas paralelas)
            for (int offset = -1; offset <= 1; ++offset) {
                SDL_RenderDrawLine(renderer, x1, y1 + offset, x2, y2 + offset);
            }
        }
        prev = seg.body;
    }

    // Desenha a ponta como um laço (círculo)
    if (!segments.empty()) {
        auto& tip = segments.back();
        if (tip.body) {
            int tx = static_cast<int>(tip.body->GetPosition().x * 30.0f) - cameraX;
            int ty = static_cast<int>(tip.body->GetPosition().y * 30.0f);
            SDL_SetRenderDrawColor(renderer, 200, 160, 90, 255);
            SDL_Rect loop = {tx - 8, ty - 8, 16, 16};
            SDL_RenderDrawRect(renderer, &loop);
        }
    }
}

b2Body* Rope::getTipBody() const {
    if (segments.empty()) return nullptr;
    return segments.back().body;
}

void Rope::attachTo(b2World& world, b2Body* target) {
    if (segments.empty() || !target) return;

    b2Body* tipBody = segments.back().body;
    if (!tipBody) return;

    b2RevoluteJointDef jointDef;
    jointDef.Initialize(tipBody, target, target->GetPosition());
    jointDef.collideConnected = false;
    world.CreateJoint(&jointDef);

    attached = true;
}