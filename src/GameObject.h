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


// General object methods
Object *GO_CreateObject(char *name, double x, double y, double rot, Object *parent);
void GO_LogObjectTree(Object *root, FILE *outfile);
Object *GO_GetParent(Object *object);
Object *GO_GetChild(Object *object, char *name);
void GO_AddChild(Object *parent, Object *child);

Object *GO_GetComponentOwner(Component *component);
Component *GO_GetComponent(Object *obj, int componentType);
Component *GO_GetComponentFromOwner(Component *comp, int componentType);
Component *GO_GetComponentFromParent(Component *comp, int componentType);
int GO_HasComponent(Object *obj, int componentType);
void GO_AddComponent(Object *obj, Component *component);

#endif //GAMEOBJECT_H
