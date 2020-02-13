#ifndef COLLIDER_H
#define COLLIDER_H

#include "Typedefs.h"
#include "Polygon.h"
#include "GameObject.h"
#include "Interval.h"

// Real time collision step methods
//TODO: need to make collision work. Do we need a separate collider type for bullits?
void CO_CheckCollision(Object *obj0, Object *obj1);
Interval CO_ProjectOnAxis(Object *obj, Real2 axis);
int CO_MayCollide(Object *obj1, Object *obj2);
Real2 CO_PolygonGetFirstVertex(Object *obj1);

// Ship methods
void CO_GenerateShipCollider(Object *ship);
void CO_ShipHandleCollision(Object *ship, Object *other);

// Projectile methods
void CO_GenerateProjectileCollider(Object *projectile);

#endif //COLLIDER_H
