#include "../include/DynamicObject.h"

DynamicObject::DynamicObject(b2World& world, const b2Vec2& p, const DynamicBodyConfig& config)
    : PhysicsObject(p)
{
    b2BodyDef defCorpo;
    defCorpo.type = b2_dynamicBody;
    defCorpo.position.Set(p.x / P2M, p.y / P2M);
    defCorpo.fixedRotation = config.fixedRotation;
    defCorpo.bullet = config.bullet;
    defCorpo.gravityScale = config.gravityScale;
    corpo = world.CreateBody(&defCorpo);
}
