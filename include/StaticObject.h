#ifndef STATIC_OBJECT_H
#define STATIC_OBJECT_H

#include "PhysicsObject.h"

class StaticObject : public PhysicsObject {
protected:
    StaticObject(b2World& world, const b2Vec2& p);
};

#endif
