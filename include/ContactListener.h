#ifndef CONTACT_LISTENER_H
#define CONTACT_LISTENER_H

#include <box2d/box2d.h>
#include <vector>
#include <utility>

enum class EntityType {
    PLAYER,
    BULLET_PLAYER,
    BULLET_BANDIT,
    BANDIT,
    COW,
    FENCE,
    ROPE_TIP,
    TERRAIN
};

struct EntityData {
    EntityType type;
    int id;
};

struct CollisionPair {
    EntityData* a;
    EntityData* b;
};

class GameContactListener : public b2ContactListener {
public:
    void BeginContact(b2Contact* contact) override;

    std::vector<CollisionPair> getAndClearCollisions();

private:
    std::vector<CollisionPair> pendingCollisions;
};

#endif