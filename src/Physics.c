#include "Physics.h"

#define FIELD_COUNT 6

List *PH_forcesLog = NULL;

// Local procedures
// ODE Solver methods
int    _PH_GetDim(void *objects);
void   _PH_SetNextVal(void *objects, int i, double newVal);
void   _PH_UpdateVals(void *objects);
double _PH_GetVal(void *objects, int i);
double _PH_dydt(double t, ODEVector y, int ind);
Real2 _PH_GetPosFromCOM(Object *object, double rot, Real2 centerOfMass);
void _PH_ClearAllForcesRec(Object *root);
void _PH_CheckPhysicsEnabled(Object *obj);

// Getters

int PH_IsPhysicsEnabled(Object *obj)
{
	 return obj->physics.enabled;
}

Real2 PH_GetLinearVelocity(Object *obj)
{
	return obj->physics.linearVel;
}

Real2 PH_GetCenterOfMass(Object *obj)
{
	return obj->physics.centerOfMass;
}

double PH_GetLinearVelocityComponent(Object *obj, int component)
{
	switch (component) {
		case R2_X:
			return obj->physics.linearVel.x;
			break;
		case R2_Y:
			return obj->physics.linearVel.y;
			break;
	}
}

double PH_GetCenterOfMassComponent(Object *obj, int component)
{
	switch (component) {
		case R2_X:
			return obj->physics.centerOfMass.x;
			break;
		case R2_Y:
			return obj->physics.centerOfMass.y;
			break;
	}
}

double PH_GetForceAccumComponent(Object *obj, int component)
{
	switch (component) {
		case R2_X:
			return obj->physics.forceAccum.x;
			break;
		case R2_Y:
			return obj->physics.forceAccum.y;
			break;
	}
}

Real2 PH_GetForceAccum(Object *obj)
{
	return obj->physics.forceAccum;
}

double PH_GetTorqueAccum(Object *obj)
{
	return obj->physics.torqueAccum;
}

double PH_GetAngularVelocity(Object *obj)
{
	return obj->physics.angularVel;
}

double PH_GetMass(Object *obj)
{
	return obj->physics.mass;
}

double PH_GetMomentOfInertia(Object *obj)
{
	return obj->physics.momentOfInertia;
}

// Setters

void PH_SetPhysicsEnabled(Object *obj, int enabled)
{
	obj->physics.enabled = enabled;
}

void PH_SetLinearVelocity(Object *obj, Real2 linearVel)
{
	obj->physics.linearVel = linearVel;
}

void PH_SetAngularVelocity(Object *obj, double angularVel)
{
	obj->physics.angularVel = angularVel;
}

void PH_SetCenterOfMass(Object *obj, Real2 centerOfMass)
{
	obj->physics.centerOfMass = centerOfMass;
}

void PH_SetForceAccum(Object *obj, Real2 force)
{
	obj->physics.forceAccum = force;
}

void PH_SetTorqueAccum(Object *obj, double torque)
{
	obj->physics.torqueAccum = torque;
}

void PH_SetMass(Object *obj, double mass)
{
	obj->physics.mass = mass;
}

void PH_SetMomentOfInertia(Object *obj, double momentOfInertia)
{
	obj->physics.momentOfInertia = momentOfInertia;
}

void PH_UpdateShipMass(Object *ship)
{
	int x, y;
	double mass;

	mass = 0;
	for (y = 0; y < GO_ShipGetHeight(ship); y++) {
		for (x = 0; x < GO_ShipGetWidth(ship); x++) {
			mass += GO_ShipGetBlock(ship, x, y)->material->mass;
		}
	}
	ship->physics.mass = mass;
}

void PH_UpdateShipCOM(Object *ship)
{
	Real2 com;
	int x, y;

	com = (Real2) { 0, 0 };
	for (y = 0; y < GO_ShipGetHeight(ship); y++) {
		for (x = 0; x < GO_ShipGetWidth(ship); x++) {
			com.x += GO_ShipGetBlock(ship, x, y)->material->mass * (x + 0.5);
			com.y += GO_ShipGetBlock(ship, x, y)->material->mass * (y + 0.5);
		}
	}
	if (ship->physics.mass != 0) {
		com.x /= ship->physics.mass;
		com.y /= ship->physics.mass;
	}
	ship->physics.centerOfMass = com;
}

void PH_UpdateShipMOI(Object *ship)
{
	int x, y;
	double moi, distToCOM;
	Real2 com;

	moi = 0;
	com = ship->physics.centerOfMass;
	for (y = 0; y < GO_ShipGetHeight(ship); y++) {
		for (x = 0; x < GO_ShipGetWidth(ship); x++) {
			distToCOM = R2_DistSq(com, (Real2) { x + 0.5, y + 0.5 }); // Get distance squared
			moi += GO_ShipGetBlock(ship, x, y)->material->mass * (distToCOM);
		}
	}
	ship->physics.momentOfInertia = moi;
}

void PH_UpdateShipPhysicsData(Object *ship)
{
	PH_UpdateShipMass(ship);
	PH_UpdateShipCOM(ship);
	PH_UpdateShipMOI(ship);
}

void PH_UpdateObjectTree(Object *root, double deltaT)
{
	ODEVector y;
	
	y = (ODEVector) {
		_PH_GetDim,
		_PH_SetNextVal,
		_PH_UpdateVals,
		_PH_GetVal,
		root,
	};

	ode(y, 0, deltaT, _PH_dydt);
}

void PH_ApplyForce(Object *obj, Real2 force, Real2 localPos)
{
	// Point of application of force relative to center of mass
	Real2 adjustedPos;
	double newTorque;

	adjustedPos = R2_Sub(GO_PosToRootSpace(obj, localPos), GO_PosToRootSpace(obj, PH_GetCenterOfMass(obj)));
	newTorque = PH_GetTorqueAccum(obj) + adjustedPos.x * force.y - adjustedPos.y * force.x;

	PH_SetForceAccum(obj, R2_Add(PH_GetForceAccum(obj), force));
	PH_SetTorqueAccum(obj, newTorque * 180 / M_PI);

	// If debug drawing, log the force for later drawing
	if (DRAW_FORCES) {
		Force *forceLog;
		forceLog = malloc(sizeof(Force));
		*forceLog = (Force) {obj, force, localPos};
		if (PH_forcesLog == NULL) {
			PH_forcesLog = List_Append(List_Nil(), forceLog);
		} else {
			List_Append(PH_forcesLog, forceLog);
		}
	}
}

void PH_ClearForces(Object *obj)
{
	PH_SetForceAccum(obj, (Real2) {0, 0});
	PH_SetTorqueAccum(obj, 0);
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

// Local procedures
void _PH_CheckPhysicsEnabled(Object *obj)
{
	if (!PH_IsPhysicsEnabled(obj)) {
		fprintf(stderr, "Error: Physics is not enabled for object %s\n", obj->name);
		exit(1);
	}
}

void _PH_ClearAllForcesRec(Object *root)
{
	List *children;

	if (PH_IsPhysicsEnabled(root)) {
		PH_ClearForces(root);
	}

	children = root->children;
	while (!List_IsEmpty(children)) {
		PH_ClearAllForces(List_Head(children));
		children = List_Tail(children);
	}
}

int _PH_GetDim(void *objects)
{
	int count;
	Object *root;
	List *children;

	count = 0;
	root = (Object *) objects;
	children = root->children;
	while (!List_IsEmpty(children)) {
		count += FIELD_COUNT;
		children = List_Tail(children);
	}
	return count;
}

void _PH_SetNextVal(void *objects, int i, double newVal)
{
	Object *root, *child;
	List *children;
	int j, objInd;

	root = (Object *) objects;
	children = root->children;
	objInd = i / FIELD_COUNT;

	for (j = 0; j < objInd; j++) {
		children = List_Tail(children);
	}
	child = List_Head(children);
	switch (i % FIELD_COUNT) {
		case 0:
			child->physics.nextCenterOfMass.x = newVal;	
			break;
		case 1:
			child->physics.nextCenterOfMass.y = newVal;	
			break;
		case 2:
			child->physics.nextRot = newVal;
			break;
		case 3:
			child->physics.nextLinearVel.x = newVal;
			break;
		case 4:
			child->physics.nextLinearVel.y = newVal;
			break;
		case 5:
			child->physics.nextAngularVel = newVal;
			break;
		default:
			fprintf(stderr, "Error: _PH_SetNextVal: This should never be reached\n");
			exit(1);
			break;
	}
}

void _PH_UpdateVals(void *objects)
{
	Object *root, *child;
	List *children;

	root = (Object *) objects;
	children = root->children;
	while (!List_IsEmpty(children)) {
		child = List_Head(children);
		if (child->physics.enabled) {
			child->physics.prevTransform = child->transform;
			child->transform.pos = _PH_GetPosFromCOM(child, child->physics.nextRot, child->physics.nextCenterOfMass);
			child->transform.rot = child->physics.nextRot;
			child->physics.linearVel = child->physics.nextLinearVel;
			child->physics.angularVel = child->physics.nextAngularVel;
		}
		children = List_Tail(children);
	}
}

double _PH_GetVal(void *objects, int i)
{
	Object *root, *child;
	List *children;
	int j, objInd;

	root = (Object *) objects;
	children = root->children;
	objInd = i / FIELD_COUNT;

	for (j = 0; j < objInd; j++) {
		children = List_Tail(children);
	}
	child = List_Head(children);
	switch (i % FIELD_COUNT) {
		case 0:
			return GO_PosToParentSpace(child, child->physics.centerOfMass).x;	
			break;
		case 1:
			return GO_PosToParentSpace(child, child->physics.centerOfMass).y;	
			break;
		case 2:
			return child->transform.rot;
			break;
		case 3:
			return PH_GetLinearVelocityComponent(child, R2_X);
			break;
		case 4:
			return PH_GetLinearVelocityComponent(child, R2_Y);
			break;
		case 5:
			return PH_GetAngularVelocity(child);
			break;
		default:
			fprintf(stderr, "Error: _PH_GetVal: This should never be reached\n");
			exit(1);
			break;
	}
}

double _PH_dydt(double t, ODEVector y, int ind)
{
	Object *root, *child;
	List *children;
	int j, objInd;

	root = (Object *) y.objects;
	children = root->children;
	objInd = ind / FIELD_COUNT;

	for (j = 0; j < objInd; j++) {
		children = List_Tail(children);
	}
	child = List_Head(children);
	switch (ind % FIELD_COUNT) {
		case 0:
			return PH_GetLinearVelocityComponent(child, R2_X);
			break;
		case 1:
			return PH_GetLinearVelocityComponent(child, R2_Y);	
			break;
		case 2:
			return PH_GetAngularVelocity(child);
			break;
		case 3:
			return PH_GetForceAccumComponent(child, R2_X) / PH_GetMass(child);
			break;
		case 4:
			return PH_GetForceAccumComponent(child, R2_Y) / PH_GetMass(child);
			break;
		case 5:
			return PH_GetTorqueAccum(child) / PH_GetMomentOfInertia(child);
			break;
		default:
			fprintf(stderr, "Error: _PH_dydt: This should never be reached\n");
			exit(1);
			break;
	}
}

Real2 _PH_GetPosFromCOM(Object *object, double rot, Real2 centerOfMass)
{
	return R2_Add(R2_RotateDeg(R2_ScalarMult(object->physics.centerOfMass, -1), rot), centerOfMass);
}
