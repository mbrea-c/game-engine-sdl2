#ifndef PHYSICS_H
#define PHYSICS_H

#include <stdio.h>
#include "GameObject.h"
#include "List.h"
#include "ODE_Euler.h"

// Getters
int PH_IsPhysicsEnabled(Object *obj);
Real2 PH_GetLinearVelocity(Object *obj);
double PH_GetLinearVelocityComponent(Object *obj, int component);
Real2 PH_GetCenterOfMass(Object *obj);
double PH_GetCenterOfMassComponent(Object *obj, int component);
Real2 PH_GetForceAccum(Object *obj);
double PH_GetForceAccumComponent(Object *obj, int component);
double PH_GetTorqueAccum(Object *obj);
double PH_GetAngualrVelocity(Object *obj);
double PH_GetMass(Object *obj);
double PH_GetMomentOfInertia(Object *obj);

// Setters
void PH_SetPhysicsEnabled(Object *obj, int enabled);
void PH_SetLinearVelocity(Object *obj, Real2 linearVel);
void PH_SetCenterOfMass(Object *obj, Real2 centerOfMass);
void PH_SetForceAccum(Object *obj, Real2 force);
void PH_SetTorqueAccum(Object *obj, double torque);
void PH_SetAngularVelocity(Object *obj, double angularVelocity);
void PH_SetMass(Object *obj, double mass);
void PH_SetMomentOfInertia(Object *obj, double momentOfInertia);

// Ship specific procedures
void PH_UpdateShipMass(Object *ship); //Total mass
void PH_UpdateShipCOM(Object *ship); //Center of mass
void PH_UpdateShipMOI(Object *ship); //Moment of inertia
void PH_UpdateShipPhysicsData(Object *ship); // Update mass, com and moi

// General Object procedures
void PH_ApplyForce(Object *obj, Real2 force, Real2 pos);
void PH_ClearForces(Object *obj);

// Main updater
void PH_UpdateObjectTree(Object *root, double deltaT);

#endif //PHYSICS_H
