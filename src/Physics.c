#include "Physics.h"

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

void PH_SetLinearVelocity(Object *obj, Real2 newVel)
{
	obj->physics.linearVel = newVel;
}

void PH_SetAngularVelocity(Object *obj, double newVel)
{
	obj->physics.angularVel = newVel;
}

void PH_UpdateRotation(Object *obj, double deltaT)
{
	double deltaAngle;
	Real2 comOld, comNew;

	deltaAngle = deltaT * obj->physics.angularVel;	
	comOld = GO_GetParentRelativePos(obj, obj->physics.centerOfMass);
	obj->transform.rot += deltaAngle;
	comNew = GO_GetParentRelativePos(obj, obj->physics.centerOfMass);
	obj->transform.pos = R2_Sub(obj->transform.pos, R2_Sub(comNew, comOld));
}

void PH_UpdatePosition(Object *obj, double deltaT)
{
	// TODO: Note that velocity is in global coordinates, physics break otherwise
	// TODO: On second thought, let's break physics
	
	obj->transform.pos = R2_Add(
			obj->transform.pos, 
			R2_ScalarMult(obj->physics.linearVel, deltaT)
			);
}

void PH_UpdatePhysicsMotion(Object *obj, double deltaT)
{
	PH_UpdateRotation(obj, deltaT);
	PH_UpdatePosition(obj, deltaT);
}

void PH_UpdateObjectTree(Object *root, double deltaT)
{
	// Assuming root not null
	List *child;

	if (root->physics.enabled) {
		PH_UpdatePhysicsMotion(root, deltaT);
	}

	if (root->children != NULL) {
		child = root->children;
		while (child->next != NULL) {
			PH_UpdateObjectTree((Object *) child->element, deltaT);
			child = child->next;
		}
		PH_UpdateObjectTree((Object *) child->element, deltaT);
	}
}
