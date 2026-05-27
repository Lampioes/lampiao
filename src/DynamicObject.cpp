#include "../include/DynamicObject.h"

DynamicObject::DynamicObject(b2World& world, float x, float y, const DynamicBodyConfig& config)
    : PhysicsObject(x, y)
{
    b2BodyDef defCorpo;
    defCorpo.type = b2_dynamicBody;
    defCorpo.position.Set(x / P2M, y / P2M);
    defCorpo.fixedRotation = config.fixedRotation;
    defCorpo.bullet = config.bullet;
    defCorpo.gravityScale = config.gravityScale;
    corpo = world.CreateBody(&defCorpo);
}
