#ifndef PHYSICS_H
#define PHYSICS_H

#include <stdio.h>
#include "GameObject.h"
#include "List.h"

// Ship specific procedures
void PH_UpdateShipMass(Object *ship); //Total mass
void PH_UpdateShipCOM(Object *ship); //Center of mass
void PH_UpdateShipMOI(Object *ship); //Moment of inertia
void PH_UpdateShipPhysicsData(Object *ship); // Update mass, com and moi

// General Object procedures
void PH_UpdateRotation(Object *obj, double deltaT);
void PH_UpdatePosition(Object *obj, double deltaT);
void PH_UpdatePhysicsMotion(Object *obj, double deltaT);
void PH_SetLinearVelocity(Object *obj, Real2 newVel);
void PH_SetAngularVelocity(Object *obj, double newVel);

// Main updater
void PH_UpdateObjectTree(Object *root, double deltaT);

#endif //PHYSICS_H
