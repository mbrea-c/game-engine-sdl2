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
#include "CollisionDetection.h"


// General object methods
Object *GO_CreateObject(int type, char *name, double x, double y, double rot,
		void *obj, Object *parent);
void GO_LogObjectTree(Object *root, FILE *outfile);
Object *GO_GetParent(Object *object);
Object *GO_GetChild(Object *object, char *name);
void GO_AddChild(Object *parent, Object *child);
//given obj O and local space pos P, return P in local space of O's parent
Real2 GO_PosToParentSpace(Object *obj, Real2 localPos);
//given obj O and local space pos P, return P in root space
Real2 GO_PosToRootSpace(Object *obj, Real2 localPos);
//given obj O and root space pos P, return P in local space of O
Real2 GO_PosToLocalSpace(Object *obj, Real2 rootPos);
Real2 GO_PosToRootSpaceObj(Object *obj);
double GO_RotToParentSpace(Object *obj, double localRot);
double GO_RotToRootSpace(Object *obj, double localRot);
double GO_RotToLocalSpace(Object *obj, double rootRot);
double GO_RotToRootSpaceObj(Object *obj);
Transform GO_TransformToParentSpace(Object *obj, Transform localTransform);
Transform GO_TransformToRootSpace(Object *obj, Transform localTransform);
Transform GO_TransformToLocalSpace(Object *obj, Transform rootTransform);
Transform GO_TransformToRootSpaceObj(Object *obj);


// Ship procedures
Object *GO_CreateEmptyShip(char *name, double x, double y, double rot, Object *parent, int width, int height);
Block *GO_ShipGetBlock(Object *ship, int x, int y);
int GO_ShipGetHeight(Object *ship);
int GO_ShipGetWidth(Object *ship);
void GO_ShipSetBlock(Object *ship, int x, int y, Block block);
void GO_ShipSetBlockWall(Object *ship, int x, int y, int wall, int wallType);
void GO_ShipCloseWithWalls(Object *ship, int wallType);
void GO_ShipAddHole(Object *ship, Real2 damagePos);

// Projectile procedures
Object *GO_CreateProjectile(char *name, double x, double y, Object *parent, int projectileType, double size);

// Block procedures
Block GO_CreateUnwalledBlock(int type);

// Camera procedures
Object *GO_CreateCamera(double x, double y, char *name, Object *parent, double width);
Object *GO_GetCamera(void);
void GO_SetCamera(Object *camera);

// Trail procedures
Object *GO_CreateTrail(char *name, Object *parent, int length, int r, int g, int b, int a);
void GO_PushToTrail(Object *trail, Real2 globalPoint);

#endif //GAMEOBJECT_H
