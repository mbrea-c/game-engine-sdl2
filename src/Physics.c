#include "Physics.h"

#define FIELD_COUNT 6

// Local procedures
// ODE Solver methods
int    _PH_GetDim(void *objects);
void   _PH_SetNextVal(void *objects, int i, double newVal);
void   _PH_UpdateVals(void *objects);
double _PH_GetVal(void *objects, int i);
double _PH_dydt(double t, ODEVector y, int ind);
Real2 _PH_GetPosFromCOM(Object *object, double rot, Real2 centerOfMass);
void _PH_CheckPhysicsEnabled(Object *obj)
{
	if (!PH_IsPhysicsEnabled(obj)) {
		fprintf(stderr, "Error: Physics is not enabled for object %s\n", obj->name);
		exit(1);
	}
}

// Getters
//TODO: Potentially remove physics enabled check, kept there for debug

int PH_IsPhysicsEnabled(Object *obj)
{
	 return obj->physics.enabled;
}

Real2 PH_GetLinearVelocity(Object *obj)
{
	_PH_CheckPhysicsEnabled(obj);
	return obj->physics.linearVel;
}

Real2 PH_GetCenterOfMass(Object *obj)
{
	_PH_CheckPhysicsEnabled(obj);
	return obj->physics.centerOfMass;
}

double PH_GetAngualrVelocity(Object *obj)
{
	_PH_CheckPhysicsEnabled(obj);
	return obj->physics.angularVel;
}

double PH_GetMass(Object *obj)
{
	_PH_CheckPhysicsEnabled(obj);
	return obj->physics.mass;
}

double PH_GetMomentOfInertia(Object *obj)
{
	_PH_CheckPhysicsEnabled(obj);
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

// Local procedures
int _PH_GetDim(void *objects)
{
	int count;
	Object *root;
	List *children;

	count = 0;
	root = (Object *) objects;
	children = root->children;
	while (children != NULL) {
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
	while (children != NULL) {
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
			return child->physics.linearVel.x;
			break;
		case 4:
			return child->physics.linearVel.y;
			break;
		case 5:
			return child->physics.angularVel;
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
			return child->physics.linearVel.x;
			break;
		case 1:
			return child->physics.linearVel.y;	
			break;
		case 2:
			return child->physics.angularVel;
			break;
		case 3:
			return 0;
			break;
		case 4:
			return 0;
			break;
		case 5:
			return 0;
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
