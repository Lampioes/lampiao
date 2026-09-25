#ifndef DYNAMIC_OBJECT_H
#define DYNAMIC_OBJECT_H

#include "PhysicsObject.h"

struct DynamicBodyConfig {
    bool fixedRotation = false;
    bool bullet = false;
    float gravityScale = 1.0f;
};

class DynamicObject : public PhysicsObject {
protected:
    DynamicObject(b2World& world, const b2Vec2& p, const DynamicBodyConfig& config = {});
};

#endif
