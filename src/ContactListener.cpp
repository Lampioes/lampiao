#include "../include/ContactListener.h"

void GameContactListener::BeginContact(b2Contact* contact) {
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    auto ptrA = fixtureA->GetUserData().pointer;
    auto ptrB = fixtureB->GetUserData().pointer;

    if (!ptrA || !ptrB) return;

    EntityData* dataA = reinterpret_cast<EntityData*>(ptrA);
    EntityData* dataB = reinterpret_cast<EntityData*>(ptrB);

    pendingCollisions.push_back({dataA, dataB});
}

std::vector<CollisionPair> GameContactListener::getAndClearCollisions() {
    std::vector<CollisionPair> result = std::move(pendingCollisions);
    pendingCollisions.clear();
    return result;
}