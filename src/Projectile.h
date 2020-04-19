#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Typedefs.h"

// Basic
Component *PR_Create(int projectileType, double size);
void PR_Mount(Component *projectile);
void PR_Destructor(void *projectileData);

// Getters
int PR_GetType(Component *projectile);
double PR_GetMass(Component *projectile);
double PR_GetSize(Component *projectile);
LTexture *PR_GetTexture(Component *projectile);

#endif //PROJECTILE_H
