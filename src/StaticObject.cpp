#include "../include/StaticObject.h"

StaticObject::StaticObject(b2World& world, const b2Vec2& p) : PhysicsObject(p) {
    b2BodyDef defCorpo;
    defCorpo.type = b2_staticBody;
    defCorpo.position.Set(p.x / P2M, p.y / P2M);
    corpo = world.CreateBody(&defCorpo);
}
