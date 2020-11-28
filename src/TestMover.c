#include "TestMover.h"
#include "Component.h"
#include "GameObject.h"
#include "Transform.h"
#include "Input.h"

int tmDependencies[] = { COMP_TRANSFORM };

// Local declarations
void TM_Mount(Component *testMover);
void TM_Update(Component *testMover);
void TM_Destructor(void *testMoverData);
void TM_SetPoint(Component *testMover, int index, Real2 point);
void TM_IncrementNext(Component *testMover);

// Basic
Component *TM_Create(int length, RGBA color)
{
	unsigned int i;
	Component *testMover;
	List *dependenciesList;

	// Create dependencies list
	dependenciesList = List_Nil();
	for (i = 0; i < sizeof(tmDependencies)/sizeof(int); i++) {
		List_Append(dependenciesList, &tmDependencies[i]);
	}

	testMover = CM_CreateComponent(COMP_TESTMOVER, NULL, &TM_Destructor, &TM_Mount, &TM_Update, dependenciesList);
	return testMover;
}

void TM_Mount(Component *testMover) {}

void TM_Update(Component *testMover) 
{
	const Uint8 *keyStates = IN_GetKeyStates();
	Component *transform = GO_GetComponentFromOwner(testMover, COMP_TRANSFORM);
	if (keyStates[SDL_SCANCODE_A]) {
		TR_SetPos(transform, R2_Add(TR_GetPos(transform), (Real2) {-0.1, 0}));
	}
	if (keyStates[SDL_SCANCODE_W]) {
		TR_SetPos(transform, R2_Add(TR_GetPos(transform), (Real2) {0, -0.1}));
	}
	if (keyStates[SDL_SCANCODE_S]) {
		TR_SetPos(transform, R2_Add(TR_GetPos(transform), (Real2) {0, 0.1}));
	}
	if (keyStates[SDL_SCANCODE_D]) {
		TR_SetPos(transform, R2_Add(TR_GetPos(transform), (Real2) {0.1, 0}));
	}
	if (keyStates[SDL_SCANCODE_E]) {
		TR_SetRot(transform, TR_GetRot(transform) + 0.5);
	}
	if (keyStates[SDL_SCANCODE_Q]) {
		TR_SetRot(transform, TR_GetRot(transform) - 0.5);
	}
}

void TM_Destructor(void *testMoverData) {}
