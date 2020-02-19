#ifndef PHYSICS_H
#define PHYSICS_H

#include <stdio.h>
#include "GameObject.h"
#include "List.h"
#include "ODE_Euler.h"

// Getters
int PH_IsPhysicsEnabled(Object *obj);
Real2 PH_GetLinearVelocity(Object *obj);
Real2 PH_GetCenterOfMass(Object *obj);
double PH_GetAngualrVelocity(Object *obj);
double PH_GetMass(Object *obj);
double PH_GetMomentOfInertia(Object *obj);

// Setters
void PH_SetPhysicsEnabled(Object *obj, int enabled);
void PH_SetLinearVelocity(Object *obj, Real2 linearVel);
void PH_SetCenterOfMass(Object *obj, Real2 centerOfMass);
void PH_SetAngualrVelocity(Object *obj, double angularVelocity);
void PH_SetMass(Object *obj, double mass);
void PH_SetMomentOfInertia(Object *obj, double momentOfInertia);


// Ship specific procedures
void PH_UpdateShipMass(Object *ship); //Total mass
void PH_UpdateShipCOM(Object *ship); //Center of mass
void PH_UpdateShipMOI(Object *ship); //Moment of inertia
void PH_UpdateShipPhysicsData(Object *ship); // Update mass, com and moi

// General Object procedures
void PH_SetLinearVelocity(Object *obj, Real2 newVel);
void PH_SetAngularVelocity(Object *obj, double newVel);

// Main updater
void PH_UpdateObjectTree(Object *root, double deltaT);

#endif //PHYSICS_H
