#ifndef COLLIDER_H
#define COLLIDER_H

#include "Typedefs.h"
#include "Polygon.h"
#include "GameObject.h"
#include "Interval.h"

// Basic
Component *CD_CreateCollider(int type);

int CD_GetType(Component *collider);
void *CD_GetCollider(Component *collider);

// Setters
void CD_ResetCollider(Component *collider, int type);

void CD_ComputeCollisions(void);





// Real time collision step methods
//TODO: need to make collision work. Do we need a separate collider type for bullits?
//TODO: REDO AS COMPONENT
//void CD_CheckCollision(Object *obj0, Object *obj1);
//Interval CD_ProjectOnAxis(Object *obj, Real2 axis);
//int CD_MayCollide(Object *obj1, Object *obj2);
//Real2 CD_PolygonGetFirstVertex(Object *obj1);

// Ship methods
//void CD_ShipHandleCollision(Object *ship, Object *other);

// Projectile methods
//void CD_GenerateProjectileCollider(Object *projectile);

#endif //COLLIDER_H
