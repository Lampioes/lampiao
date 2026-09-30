#ifndef PHYSICS_OBJECT_H
#define PHYSICS_OBJECT_H

#include <box2d/box2d.h>

#include "GameObject.h"

class PhysicsObject : public GameObject {
public:
    b2Body* getBody() const { return corpo; }

    virtual void destroyBody(b2World& world);

protected:
    explicit PhysicsObject(const b2Vec2& p) : GameObject(p) {}

    b2Body* corpo = nullptr;
};

#endif
