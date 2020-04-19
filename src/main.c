#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <unistd.h>
#include "LTexture.h"
#include "List.h"
#include "Graphics.h"
#include "GameObject.h"
#include "Transform.h"
#include "Assets.h"
#include "Physics.h"
#include "Collider.h"
#include "Camera.h"
#include "Ship.h"
#include "Input.h"
#include "Debug.h"

#define FPS_CAP 60
#define TICKS_PER_FRAME 1000 / FPS_CAP

int main(int argc, char **argv)
{
	int quit, willUpdate, stepByStep;
	double timeCoefficient;
	Uint32 startTime, frameTime;
	SDL_Event event;
	Object *root = GO_CreateObject("top", 0, 0, 0, NULL);

	Object *camera = GO_CreateObject("camerita", 0, 0, 0, root);
	Component *cameraComponent = CA_Create(50);
	printf("Adding Camera component\n");
	GO_AddComponent(camera, cameraComponent);

	GR_Init(camera);
	Object *ship = GO_CreateObject("shipi", 7, 7, 0, root); 
	GO_AddComponent(ship, PH_CreatePhysicsZeroed());
	GO_AddComponent(ship, CD_CreateCollider(COLL_POLYGON));
	GO_AddComponent(ship, SH_CreateShipEmpty(10, 15));
	Component *shipComp = GO_GetComponent(ship, COMP_SHIP);

	printf("Got to the end of ship creation\n");

	SH_SetBlock(shipComp, 0,0, SH_CreateUnwalledBlock(BLOCK_TEST));
	printf("Created block\n");
	SH_SetBlock(shipComp, 1,0, SH_CreateUnwalledBlock(BLOCK_TEST));
	printf("Created block\n");
	SH_SetBlock(shipComp, 2,0, SH_CreateUnwalledBlock(BLOCK_TEST));
	printf("Created block\n");
	SH_SetBlock(shipComp, 3,0, SH_CreateUnwalledBlock(BLOCK_TEST));
	printf("Created block\n");
	SH_SetBlock(shipComp, 4,0, SH_CreateUnwalledBlock(BLOCK_TEST));
	printf("Created block\n");

	SH_UpdatePhysicsData(shipComp);
	printf("Updated ship physics data\n");
	SH_CloseWithWalls(shipComp, WALL_LIGHT);
	printf("Closed off ship walls\n");
	SH_UpdateCollider(shipComp);
	printf("Updated ship collider\n");
	
	GO_LogObjectTree(root, stderr);

	quit = 0;
	startTime = 0;
	timeCoefficient = 1;
	willUpdate = 0;
	stepByStep = 0;
	// Main loop

	List_Print(root->children);
	while (!quit) {
		startTime = SDL_GetTicks();
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quit = 1;
			} else if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_q:
						quit = 1;
						break;
					case SDLK_SPACE:
						willUpdate = 1;
						break;
					case SDLK_s:
						stepByStep = !stepByStep;
						break;
				}
			}
		}
		/*PH_ApplyForce(ship, R2_ScalarMult(R2_Normal(PH_GetCenterOfMass(ship)), 500), (Real2) {0, 0} );*/
		/*PH_ApplyForce(ship, R2_ScalarMult(R2_Normal(PH_GetCenterOfMass(ship)), -500), R2_ScalarMult(PH_GetCenterOfMass(ship), 2));*/
		if (willUpdate || !stepByStep) {
			PH_ApplyForce(
					GO_GetComponent(ship, COMP_PHYSICS),
					R2_ScalarMult(R2_Sub(TR_PosToRootSpace(TR_GetFromObj(camera), IN_GetMouseCameraPos()), TR_GetPos(TR_GetFromObj(ship))), 50), (Real2) {0,0});
			PH_UpdateObjectTree(root, (double) timeCoefficient * TICKS_PER_FRAME / 1000.0);
			/*if (CD_MayCollide(ship, bullet)) {*/
				/*printf("BOOM BAM CRASH BULLIT HIT!!!1!\n");*/
				/*GO_ShipAddHole(ship, GO_PosToLocalSpace(ship, GO_PosToRootSpace(bullet, CD_PolygonGetFirstVertex(bullet))));*/
			/*}*/
			GR_Render(root);
			PH_ClearAllForces(root);
			willUpdate = 0;
		}
		frameTime = SDL_GetTicks() - startTime;
		if (frameTime < TICKS_PER_FRAME) {
			SDL_Delay(TICKS_PER_FRAME - frameTime);
		}

		/*printf("FRAME TIME: %d\n", frameTime);*/
	}
	return 0;
}

