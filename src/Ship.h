#ifndef SHIP_H
#define SHIP_H

#include "Typedefs.h"

// Basic
Component *SH_CreateShipEmpty(int width, int height);

// Getters
int SH_GetHeight(Component *ship);
int SH_GetWidth(Component *ship);
Block *SH_GetBlocks(Component *ship);
List *SH_GetHoles(Component *ship);
Block *SH_GetBlock(Component *ship, int x, int y);
// Setters
void SH_SetBlock(Component *ship, int x, int y, Block block);
void SH_SetBlockWall(Component *ship, int x, int y, int wall, int wallType);

// Ship specific procedures
void SH_CloseWithWalls(Component *ship, int wallType);
//TODO: Potentially simplify system: each block/wall has amount of damage
void SH_AddHole(Component *ship, Real2 damagePos);
void SH_UpdateMass(Component *ship); //Total mass
void SH_UpdateCOM(Component *ship); //Center of mass
void SH_UpdateMOI(Component *ship); //Moment of inertia
void SH_UpdatePhysicsData(Component *ship); // Update mass, com and moi
void SH_UpdateCollider(Component *ship);

// TODO: should this be in its own file????
// Block procedures
Block SH_CreateUnwalledBlock(int type);

#endif //SHIP_H
