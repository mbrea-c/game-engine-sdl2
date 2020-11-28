#include "TestMouseForce.h"
#include "Typedefs.h"
#include "Component.h"
#include "GameObject.h"
#include "Transform.h"
#include "Physics.h"
#include "Input.h"

int tmfDependencies[] = { COMP_TRANSFORM, COMP_PHYSICS };

// Local declarations
void TMF_Mount(Component *testMover);
void TMF_Update(Component *testMover);
void TMF_Destructor(void *testMoverData);

// Basic
Component *TMF_Create(Real2 localDragPoint, double springConstant)
{
	unsigned int i;
	Component *testMouseForce;
	TestMouseForceData *testMouseForceData;
	List *dependenciesList;

	// Create dependencies list
	dependenciesList = List_Nil();
	for (i = 0; i < sizeof(tmfDependencies)/sizeof(int); i++) {
		List_Append(dependenciesList, &tmfDependencies[i]);
	}

	testMouseForceData = malloc(sizeof(TestMouseForceData));
	testMouseForceData->localDragPoint = localDragPoint;
	testMouseForceData->springConstant = springConstant;

	testMouseForce = CM_CreateComponent(COMP_TESTMOUSEFORCE, testMouseForceData, &TMF_Destructor, &TMF_Mount, &TMF_Update, dependenciesList);
	return testMouseForce;
}

void TMF_Mount(Component *testMouseForce) {}

void TMF_Update(Component *testMouseForce) 
{
	Real2 forceVector;
	Component *currentObjectTrasform;

	currentObjectTrasform = GO_GetComponentFromOwner(testMouseForce, COMP_TRANSFORM);

	forceVector = R2_Sub(IN_GetMouseRootPos(), TR_PosToRootSpace(currentObjectTrasform, TMF_GetLocalDragPoint(testMouseForce)));
	forceVector = R2_ScalarMult(forceVector, TMF_GetSpringConstant(testMouseForce));

	PH_ApplyForce(GO_GetComponentFromOwner(testMouseForce, COMP_PHYSICS), forceVector, TMF_GetLocalDragPoint(testMouseForce));
}

void TMF_Destructor(void *testMouseForceData)
{
	free(testMouseForceData);
}

// Getters
Real2 TMF_GetLocalDragPoint(Component *testMouseForce)
{
	return ((TestMouseForceData *) (testMouseForce->componentData))->localDragPoint;
}

double TMF_GetSpringConstant(Component *testMouseForce)
{
	return ((TestMouseForceData *) (testMouseForce->componentData))->springConstant;
}

// Setters
void TMF_SetLocalDragPoint(Component *testMouseForce, Real2 localDragPoint)
{
	((TestMouseForceData *) (testMouseForce->componentData))->localDragPoint = localDragPoint;
}
