#ifndef COLLIDER_H
#define COLLIDER_H

#include "Typedefs.h"
#include "Polygon.h"
#include "GameObject.h"

// Real time collision step methods
void CO_CheckCollision(Object *obj0, Object *obj1)
{
//TODO: need to make collision work. Do we need a separate collider type for bullits?
}

// Ship methods
void CO_GenerateShipCollider(Object *ship);

#endif //COLLIDER_H
