#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string.h>
#include <stdio.h>
#include "Typedefs.h"
#include "LTexture.h"
#include "Real2.h"
#include "List.h"
#include "Assets.h"
#include "Physics.h"
#include "Collider.h"


// General object methods
Object *GO_CreateObject(int type, char *name, double x, double y, double rot,
		void *obj, Object *parent);
void GO_LogObjectTree(Object *root, FILE *outfile);
Object *GO_GetParent(Object *object);
Object *GO_GetChild(Object *object, char *name);
void GO_AddChild(Object *parent, Object *child);
Transform GO_GetRootTransform(Object *object);
Transform GO_GetRelativeTransform_T(Transform *reference, Transform *target);
Real2 GO_GetParentRelativePos(Object *object, Real2 pos);
Real2 GO_GetRootPositionFrom(Object *reference, Real2 localPos);
Real2 GO_GetLocalPositionTo(Object *target, Real2 globalPos);

// Ship procedures
Object *GO_CreateEmptyShip(char *name, double x, double y, double rot, Object *parent, int width, int height);
Block *GO_ShipGetBlock(Object *ship, int x, int y);
int GO_ShipGetHeight(Object *ship);
int GO_ShipGetWidth(Object *ship);
void GO_ShipSetBlock(Object *ship, int x, int y, Block block);
void GO_ShipSetBlockWall(Object *ship, int x, int y, int wall, int wallType);
void GO_ShipCloseWithWalls(Object *ship, int wallType);

// Block procedures
Block GO_CreateUnwalledBlock(int type);

// Camera procedures
Object *GO_CreateCamera(double x, double y, char *name, Object *parent, double width);
Object *GO_GetCamera(void);
void GO_SetCamera(Object *camera);

// Trail procedures
Object *GO_CreateTrail(char *name, Object *parent, int length);
void GO_PushToTrail(Object *trail, Real2 globalPoint);

#endif //GAMEOBJECT_H
