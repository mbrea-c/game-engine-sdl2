#ifndef PHYSICS_H
#define PHYSICS_H

#include <stdio.h>
#include "Typedefs.h"
#include "GameObject.h"
#include "List.h"
#include "ODE_Euler.h"
#include "Graphics.h"

// Basic component methods
Component *PH_CreatePhysicsZeroed();

// Getters
Real2 PH_GetLinearVelocity(Component *physics);
double PH_GetLinearVelocityComponent(Component *physics, int axis);
Real2 PH_GetCenterOfMass(Component *physics);
double PH_GetCenterOfMassComponent(Component *physics, int axis);
Real2 PH_GetForceAccum(Component *physics);
double PH_GetForceAccumComponent(Component *physics, int axis);
double PH_GetTorqueAccum(Component *physics);
double PH_GetAngularVelocity(Component *physics);
double PH_GetMass(Component *physics);
double PH_GetMomentOfInertia(Component *physics);

// Setters
void PH_SetLinearVelocity(Component *physics, Real2 linearVel);
void PH_SetLinearVelocityComponent(Component *physics, double linearVel, int axis);
void PH_SetCenterOfMass(Component *physics, Real2 centerOfMass);
void PH_SetCenterOfMassComponent(Component *physics, double centerOfMass, int axis);
void PH_SetForceAccum(Component *physics, Real2 force);
void PH_SetForceAccumComponent(Component *physics, double forceAccum, int axis);
void PH_SetTorqueAccum(Component *physics, double torque);
void PH_SetAngularVelocity(Component *physics, double angularVelocity);
void PH_SetMass(Component *physics, double mass);
void PH_SetMomentOfInertia(Component *physics, double momentOfInertia);


// General Object procedures
void PH_ApplyForce(Component *physics, Real2 force, Real2 pos);
void PH_ClearForces(Component *physics);
//TODO: change this when keep track of physics objects is done 
void PH_ClearAllForces(Object *root);
List *PH_GetForcesLog(void);

// Main updater
void PH_UpdateAllObjects(double deltaT);

// Debug
void PH_LogPhysicsData(Component *physics);

#endif //PHYSICS_H
