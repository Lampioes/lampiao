#include "../include/ContactListener.h"

void GameContactListener::BeginContact(b2Contact* contact) {
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    auto ptrA = fixtureA->GetUserData().pointer;
    auto ptrB = fixtureB->GetUserData().pointer;

    if (!ptrA || !ptrB) return;

    EntityData* dadosA = reinterpret_cast<EntityData*>(ptrA);
    EntityData* dadosB = reinterpret_cast<EntityData*>(ptrB);

    colisoesPendentes.push_back({dadosA, dadosB});
}

std::vector<CollisionPair> GameContactListener::getAndClearCollisions() {
    std::vector<CollisionPair> resultado = std::move(colisoesPendentes);
    colisoesPendentes.clear();
    return resultado;
}
