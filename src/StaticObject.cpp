#include "../include/StaticObject.h"

StaticObject::StaticObject(b2World& world, float x, float y) : PhysicsObject(x, y) {
    b2BodyDef defCorpo;
    defCorpo.type = b2_staticBody;
    defCorpo.position.Set(x / P2M, y / P2M);
    corpo = world.CreateBody(&defCorpo);
}
