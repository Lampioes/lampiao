#include "../include/PhysicsObject.h"
#include "../include/ContactListener.h"

#include <bit>

void PhysicsObject::destroyBody(b2World& world) {
    if (!corpo) return;

    b2Fixture* f = corpo->GetFixtureList();
    while (f) {
        if (auto ponteiro = f->GetUserData().pointer) delete std::bit_cast<DadosEntidade*>(ponteiro);
        f = f->GetNext();
    }

    world.DestroyBody(corpo);
    corpo = nullptr;
}
