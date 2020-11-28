#include "Physics.h"
#include "Component.h"
#include "Transform.h"

#define FIELD_COUNT 6

List *PH_forcesLog = NULL;
int physicsDependencies[] = { COMP_TRANSFORM };

List *gPhysicsList = NULL;

// DECLARATIONS (Local procedures)
void PH_Mount(Component *physics);
void PH_Update(Component *physics);
void PH_Destructor(void *physicsData);

// ODE Solver methods
int    _PH_GetDim();
void   _PH_SetNextVal(int i, double newVal);
void   _PH_UpdateVals();
double _PH_GetVal(int i);
double _PH_dydt(double t, ODEVector y, int ind);
Real2 _PH_GetNextPos(Component *physics);
void _PH_ClearAllForcesRec(Object *root);

// Private getters and setters (these need not be seen outside this file)
// Getters
Real2 PH_GetNextLinearVelocity(Component *physics);
double PH_GetNextLinearVelocityComponent(Component *physics, int axis);
//TODO: Figure out what the hell this does
Real2 PH_GetNextCenterOfMass(Component *physics); 
double PH_GetNextCenterOfMassComponent(Component *physics, int axis);
double PH_GetNextAngularVelocity(Component *physics);
double PH_GetNextRot(Component *physics);
double PH_GetPrevRot(Component *physics);
Real2 PH_GetPrevPos(Component *physics);
double PH_GetPrevPosComponent(Component *physics, int axis);

// Setters
void PH_SetNextLinearVelocity(Component *physics, Real2 linearVel);
void PH_SetNextLinearVelocityComponent(Component *physics, double linearVel, int axis);
void PH_SetNextCenterOfMass(Component *physics, Real2 centerOfMass);
void PH_SetNextCenterOfMassComponent(Component *physics, double centerOfMass, int axis);
void PH_SetNextAngularVelocity(Component *physics, double angularVelocity);
void PH_SetNextRot(Component *physics, double rot);
void PH_SetPrevRot(Component *physics, double rot);
void PH_SetPrevPos(Component *physics, Real2 pos);
void PH_SetPrevPosComponent(Component *physics, double pos, int axis);

// DEFINITIONS 
// Basic

//TODO: Keep track of existing physics objects
Component *PH_CreatePhysicsZeroed()
{
	unsigned int i;	
	List *dependenciesList = List_Nil();
	for (i = 0; i < sizeof(physicsDependencies)/sizeof(int); i++) {
		List_Append(dependenciesList, &physicsDependencies[i]);
	}
	PhysicsData *physicsData = malloc(sizeof(PhysicsData));
	physicsData->mass = 1;
	physicsData->momentOfInertia = 1;
	physicsData->linearVel = (Real2) {0, 0};
	physicsData->angularVel = 0;
	physicsData->centerOfMass = (Real2) {0, 0};
	// Needed for physics updaters
	physicsData->nextLinearVel = (Real2) {0, 0};
	physicsData->nextAngularVel = 0;
	physicsData->nextRot = 0;
	physicsData->nextCenterOfMass = (Real2) {0, 0};
	// Accumulators
	physicsData->forceAccum = (Real2) {0, 0};
	physicsData->torqueAccum = 0;
	// Old transform data for collisions
	physicsData->prevPos = (Real2) {0, 0};
	physicsData->prevRot = 0;
	Component *component = CM_CreateComponent(COMP_PHYSICS, physicsData, &PH_Destructor, &PH_Mount, &PH_Update, dependenciesList);
	return component;
}

void PH_Mount(Component *physics) {
	// Init trail list if not already
	if (gPhysicsList == NULL) gPhysicsList = List_Nil();
	List_Append(gPhysicsList, physics);
}
void PH_Update(Component *physics) {}

void PH_Destructor(void *physicsData)
{
	int i;
	List *plist;

	i = 0;
	plist = gPhysicsList;
	LOOP_OVER(plist) {
		if (((Component *)List_Head(plist))->componentData == physicsData) {
			List_Delete(gPhysicsList, i);
			break;
		}
		i++;
	}

	free(physicsData);
}

// Getters

Real2 PH_GetLinearVelocity(Component *physics)
{
	return ((PhysicsData *) physics->componentData)->linearVel;
}

double PH_GetLinearVelocityComponent(Component *physics, int axis)
{
	return R2_GetComponent(PH_GetLinearVelocity(physics), axis);
}

Real2 PH_GetCenterOfMass(Component *physics)
{
	return ((PhysicsData *) physics->componentData)->centerOfMass;
}

double PH_GetCenterOfMassComponent(Component *physics, int axis)
{
	return R2_GetComponent(PH_GetCenterOfMass(physics), axis);
}

Real2 PH_GetForceAccum(Component *physics)
{
	return ((PhysicsData *) physics->componentData)->forceAccum;
}

double PH_GetForceAccumComponent(Component *physics, int axis)
{
	return R2_GetComponent(PH_GetForceAccum(physics), axis);
}

double PH_GetTorqueAccum(Component *physics)
{
	return ((PhysicsData *) physics->componentData)->torqueAccum;
}

double PH_GetAngularVelocity(Component *physics)
{
	return ((PhysicsData *) physics->componentData)->angularVel;
}

double PH_GetMass(Component *physics)
{
	return ((PhysicsData *) physics->componentData)->mass;
}

double PH_GetMomentOfInertia(Component *physics)
{
	return ((PhysicsData *) physics->componentData)->momentOfInertia;
} 

// Setters

void PH_SetLinearVelocity(Component *physics, Real2 linearVel)
{
	((PhysicsData *) physics->componentData)->linearVel = linearVel;
}

void PH_SetLinearVelocityComponent(Component *physics, double linearVel, int axis)
{
	switch (axis) {
	case R2_X:
		((PhysicsData *) physics->componentData)->linearVel.x = linearVel;
		break;
	case R2_Y:
		((PhysicsData *) physics->componentData)->linearVel.y = linearVel;
		break;
	}
}

void PH_SetCenterOfMass(Component *physics, Real2 centerOfMass)
{
	((PhysicsData *) physics->componentData)->centerOfMass = centerOfMass;
}

void PH_SetCenterOfMassComponent(Component *physics, double centerOfMass, int axis)
{
	switch (axis) {
	case R2_X:
		((PhysicsData *) physics->componentData)->centerOfMass.x = centerOfMass;
		break;
	case R2_Y:
		((PhysicsData *) physics->componentData)->centerOfMass.y = centerOfMass;
		break;
	}
}

void PH_SetForceAccum(Component *physics, Real2 force)
{
	((PhysicsData *) physics->componentData)->forceAccum = force;
}

void PH_SetForceAccumComponent(Component *physics, double forceAccum, int axis)
{
	switch (axis) {
	case R2_X:
		((PhysicsData *) physics->componentData)->forceAccum.x = forceAccum;
		break;
	case R2_Y:
		((PhysicsData *) physics->componentData)->forceAccum.y = forceAccum;
		break;
	}
}

void PH_SetTorqueAccum(Component *physics, double torque)
{
	((PhysicsData *) physics->componentData)->torqueAccum = torque;
}

void PH_SetAngularVelocity(Component *physics, double angularVelocity)
{
	((PhysicsData *) physics->componentData)->angularVel = angularVelocity;
}

void PH_SetMass(Component *physics, double mass)
{
	if (mass == 0) mass = 0.00000000001; // Hack to prevent stupid physics bugs
	((PhysicsData *) physics->componentData)->mass = mass;
}

void PH_SetMomentOfInertia(Component *physics, double momentOfInertia)
{
	((PhysicsData *) physics->componentData)->momentOfInertia = momentOfInertia;
}

void PH_UpdateAllObjects(double deltaT)
{
	ODEVector y;
	
	y = (ODEVector) {
		_PH_GetDim,
		_PH_SetNextVal,
		_PH_UpdateVals,
		_PH_GetVal,
	};

	// Initialize physics body list if not yet done
	if (gPhysicsList == NULL) gPhysicsList = List_Nil();
	ode(y, 0, deltaT, _PH_dydt);
}

void PH_ApplyForce(Component *physics, Real2 force, Real2 localPos)
{
	// Point of application of force relative to center of mass
	Real2 adjustedPos;
	double newTorque;
	Component *transform;

	transform = GO_GetComponent(GO_GetComponentOwner(physics), COMP_TRANSFORM);

	adjustedPos = R2_Sub(TR_PosToRootSpace(transform, localPos), TR_PosToRootSpace(transform, PH_GetCenterOfMass(physics)));
	newTorque = PH_GetTorqueAccum(physics) + adjustedPos.x * force.y - adjustedPos.y * force.x;

	PH_SetForceAccum(physics, R2_Add(PH_GetForceAccum(physics), force));
	PH_SetTorqueAccum(physics, newTorque * 180 / M_PI);

	// If debug drawing, log the force for later drawing
	if (DRAW_FORCES) {
		Force *forceLog;
		forceLog = malloc(sizeof(Force));
		*forceLog = (Force) {physics, force, localPos};
		if (PH_forcesLog == NULL) {
			PH_forcesLog = List_Append(List_Nil(), forceLog);
		} else {
			List_Append(PH_forcesLog, forceLog);
		}
	}
}

void PH_ClearForces(Component *physics)
{
	PH_SetForceAccum(physics, (Real2) {0, 0});
	PH_SetTorqueAccum(physics, 0);
}

void PH_ClearAllForces(Object *root)
{
	List *next;

	_PH_ClearAllForcesRec(root);

	// Clear forces log
	if (PH_forcesLog == NULL) PH_forcesLog = List_Nil();
	while (!List_IsEmpty(PH_forcesLog)) {
		next = List_Tail(PH_forcesLog);
		free(List_Head(PH_forcesLog));
		free(PH_forcesLog);
		PH_forcesLog = next;
	}
}

List *PH_GetForcesLog(void)
{
	return PH_forcesLog;
}

void PH_LogPhysicsData(Component *physics)
{
	printf("------physics(%s)------\n", GO_GetComponentOwner(physics)->name);
	printf("mass: %f\n", PH_GetMass(physics));
	printf("momentOfInertia: %f\n", PH_GetMomentOfInertia(physics));
	printf("angularVelocity: %f\n", PH_GetAngularVelocity(physics));
	printf("linearVelocity: (%f,%f)\n", PH_GetLinearVelocityComponent(physics, R2_X), PH_GetLinearVelocityComponent(physics, R2_Y));
	printf("centerOfMass: (%f,%f)\n", PH_GetCenterOfMassComponent(physics, R2_X), PH_GetCenterOfMassComponent(physics, R2_Y));
	printf("nextAngularVel: %f\n", PH_GetNextAngularVelocity(physics));
	printf("nextRot: %f\n", PH_GetNextRot(physics));
	printf("nextCenterOfMass: (%f,%f)\n", PH_GetNextCenterOfMassComponent(physics, R2_X), PH_GetNextCenterOfMassComponent(physics, R2_Y));
	printf("nextLinearVel: (%f,%f)\n", PH_GetNextLinearVelocityComponent(physics, R2_X), PH_GetNextLinearVelocityComponent(physics, R2_Y));
	printf("torqueAccum: %f\n", PH_GetTorqueAccum(physics));
	printf("forceAccum: (%f,%f)\n", PH_GetForceAccumComponent(physics, R2_X), PH_GetForceAccumComponent(physics, R2_Y));
	printf("prevRot: %f\n", PH_GetPrevRot(physics));
	printf("prevPos: (%f,%f)\n", PH_GetPrevPosComponent(physics, R2_X), PH_GetPrevPosComponent(physics, R2_Y));
	printf("--------------------------\n");
}

// Local procedures

// Getters
Real2 PH_GetNextLinearVelocity(Component *physics)
{
	return ((PhysicsData *) physics->componentData)->nextLinearVel;
}

double PH_GetNextLinearVelocityComponent(Component *physics, int axis)
{
	return R2_GetComponent(((PhysicsData *) physics->componentData)->linearVel, axis);
}

Real2 PH_GetNextCenterOfMass(Component *physics)
{
	return ((PhysicsData *) physics->componentData)->nextCenterOfMass;
}

double PH_GetNextCenterOfMassComponent(Component *physics, int axis)
{
	return R2_GetComponent(((PhysicsData *) physics->componentData)->nextCenterOfMass, axis);
}

double PH_GetNextAngularVelocity(Component *physics)
{
	return ((PhysicsData *) physics->componentData)->nextAngularVel;
}

double PH_GetNextRot(Component *physics)
{
	return ((PhysicsData *) physics->componentData)->nextRot;
}

double PH_GetPrevRot(Component *physics)
{
	return ((PhysicsData *) physics->componentData)->prevRot;
}

Real2 PH_GetPrevPos(Component *physics)
{
	return ((PhysicsData *) physics->componentData)->prevPos;
}

double PH_GetPrevPosComponent(Component *physics, int axis)
{
	return R2_GetComponent(((PhysicsData *) physics->componentData)->prevPos, axis);
}

// Setters
void PH_SetNextLinearVelocity(Component *physics, Real2 linearVel)
{
	((PhysicsData *) physics->componentData)->nextLinearVel = linearVel;
}

void PH_SetNextLinearVelocityComponent(Component *physics, double linearVel, int axis)
{
	R2_SetComponent(&((PhysicsData *) physics->componentData)->nextLinearVel, linearVel, axis);
}

void PH_SetNextCenterOfMass(Component *physics, Real2 centerOfMass)
{
	((PhysicsData *) physics->componentData)->nextCenterOfMass = centerOfMass;
}

void PH_SetNextCenterOfMassComponent(Component *physics, double centerOfMass, int axis)
{
	R2_SetComponent(&((PhysicsData *) physics->componentData)->nextCenterOfMass, centerOfMass, axis);
}

void PH_SetNextAngularVelocity(Component *physics, double angularVelocity)
{
	((PhysicsData *) physics->componentData)->nextAngularVel = angularVelocity;
}

void PH_SetNextRot(Component *physics, double rot)
{
	((PhysicsData *) physics->componentData)->nextRot = rot;
}

void PH_SetPrevRot(Component *physics, double rot)
{
	((PhysicsData *) physics->componentData)->prevRot = rot;
}

void PH_SetPrevPos(Component *physics, Real2 pos)
{
	((PhysicsData *) physics->componentData)->prevPos = pos;
}

void PH_SetPrevPosComponent(Component *physics, double pos, int axis)
{
	R2_SetComponent(&((PhysicsData *) physics->componentData)->prevPos, pos, axis);
}


//TODO: Maybe get rid or fix force logging?
void _PH_ClearAllForcesRec(Object *root)
{
	List *children;
	Component *physics;

	if ((physics = GO_GetComponent(root, COMP_PHYSICS)) != NULL) {
		PH_ClearForces(physics);
	}

	children = root->children;
	while (!List_IsEmpty(children)) {
		PH_ClearAllForces(List_Head(children));
		children = List_Tail(children);
	}
}

int _PH_GetDim()
{
	return List_GetLength(gPhysicsList)*FIELD_COUNT;
}

void _PH_SetNextVal(int i, double newVal)
{
	Component *childPhysics;
	int objInd;

	objInd = i / FIELD_COUNT;

	childPhysics = List_GetFromIndex(gPhysicsList, objInd);
	switch (i % FIELD_COUNT) {
		case 0:
			PH_SetNextCenterOfMassComponent(childPhysics, newVal, R2_X);
			break;
		case 1:
			PH_SetNextCenterOfMassComponent(childPhysics, newVal, R2_Y);
			break;
		case 2:
			PH_SetNextRot(childPhysics, newVal);
			break;
		case 3:
			PH_SetNextLinearVelocityComponent(childPhysics, newVal, R2_X);
			break;
		case 4:
			PH_SetNextLinearVelocityComponent(childPhysics, newVal, R2_Y);
			break;
		case 5:
			PH_SetNextAngularVelocity(childPhysics, newVal);
			break;
		default:
			fprintf(stderr, "Error: _PH_SetNextVal: This should never be reached\n");
			exit(1);
			break;
	}
}

void _PH_UpdateVals()
{
	Component *childPhysics, *childTransform;
	List *plist;

	plist = gPhysicsList;
	LOOP_OVER(plist) {
		childPhysics = List_Head(plist);
		childTransform = GO_GetComponentFromOwner(childPhysics, COMP_TRANSFORM);
		PH_SetPrevPos(childPhysics, TR_GetPos(childTransform));
		TR_SetPos(childTransform, _PH_GetNextPos(childPhysics));
		TR_SetRot(childTransform, PH_GetNextRot(childPhysics));
		PH_SetLinearVelocity(childPhysics, PH_GetNextLinearVelocity(childPhysics));
		PH_SetAngularVelocity(childPhysics, PH_GetNextAngularVelocity(childPhysics));
	}
}

double _PH_GetVal(int i)
{
	Component *childPhysics, *childTransform;
	int objInd;

	objInd = i / FIELD_COUNT;

	childPhysics = List_GetFromIndex(gPhysicsList, objInd);
	childTransform = GO_GetComponentFromOwner(childPhysics, COMP_TRANSFORM);

	switch (i % FIELD_COUNT) {
		case 0:
			return TR_PosToParentSpace(childTransform, PH_GetCenterOfMass(childPhysics)).x;	
			break;
		case 1:
			return TR_PosToParentSpace(childTransform, PH_GetCenterOfMass(childPhysics)).y;	
			break;
		case 2:
			return TR_GetRot(childTransform);
			break;
		case 3:
			return PH_GetLinearVelocityComponent(childPhysics, R2_X);
			break;
		case 4:
			return PH_GetLinearVelocityComponent(childPhysics, R2_Y);
			break;
		case 5:
			return PH_GetAngularVelocity(childPhysics);
			break;
		default:
			fprintf(stderr, "Error: _PH_GetVal: This should never be reached\n");
			exit(1);
			break;
	}
}

double _PH_dydt(double t, ODEVector y, int ind)
{
	Component *childPhysics;
	int objInd;

	objInd = ind / FIELD_COUNT;

	childPhysics   = List_GetFromIndex(gPhysicsList, objInd);
	switch (ind % FIELD_COUNT) {
		case 0:
			return PH_GetLinearVelocityComponent(childPhysics, R2_X);
			break;
		case 1:
			return PH_GetLinearVelocityComponent(childPhysics, R2_Y);	
			break;
		case 2:
			return PH_GetAngularVelocity(childPhysics);
			break;
		case 3:
			return PH_GetForceAccumComponent(childPhysics, R2_X) / PH_GetMass(childPhysics);
			break;
		case 4:
			return PH_GetForceAccumComponent(childPhysics, R2_Y) / PH_GetMass(childPhysics);
			break;
		case 5:
			return PH_GetTorqueAccum(childPhysics) / PH_GetMomentOfInertia(childPhysics);
			break;
		default:
			fprintf(stderr, "Error: _PH_dydt: This should never be reached\n");
			exit(1);
			break;
	}
}

Real2 _PH_GetNextPos(Component *physics)
{
	Real2 nextCenterOfMass, centerOfMass;
	double rot;

	centerOfMass = PH_GetCenterOfMass(physics);
	nextCenterOfMass = PH_GetNextCenterOfMass(physics);
	rot = PH_GetNextRot(physics);
	return R2_Add(R2_RotateDeg(R2_ScalarMult(centerOfMass, -1), rot), nextCenterOfMass);
}
