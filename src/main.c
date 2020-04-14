#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <unistd.h>
#include "LTexture.h"
#include "List.h"
#include "Graphics.h"
#include "GameObject.h"
#include "Assets.h"
#include "Physics.h"
#include "CollisionDetection.h"
#include "Input.h"

#define FPS_CAP 60
#define TICKS_PER_FRAME 1000 / FPS_CAP

int main(int argc, char **argv)
{
	int quit, willUpdate, stepByStep;
	double timeCoefficient;
	Uint32 startTime, frameTime;
	SDL_Event event;
	GR_Init();
	Object *root = GO_CreateObject(OBJ_WORLD_NODE, "top", 0, 0, 0, NULL, NULL);
	Object *camera = GO_CreateCamera(0, 0, "camerita", root, 30);
	Object *ship = GO_CreateEmptyShip("shipi", 7, 7, 0, root, 10, 15); 
	Object *ship2 = GO_CreateEmptyShip("shipi2", 15, 10, 0, root, 10, 15); 
	Object *trail = GO_CreateTrail("shipi trail", root, 1000, 0xff, 0x00, 0x00, 0xff);
	Object *bulittrail = GO_CreateTrail("bulit trail", root, 3, 0xff, 0xff, 0x00, 0xff);
	Object *bullet = GO_CreateProjectile("bullit", 0, 16, root, PROJ_BULLET, 0.3);
	GO_ShipSetBlock(ship, 0,0, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship, 0,1, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship, 1,0, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship, 1,1, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship, 2,1, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship, 3,1, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship, 4,1, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship, 4,0, GO_CreateUnwalledBlock(BLOCK_TEST));

	GO_ShipSetBlock(ship2, 0,0, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship2, 0,1, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship2, 1,0, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship2, 1,1, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship2, 2,0, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship2, 2,1, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship2, 3,1, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship2, 3,2, GO_CreateUnwalledBlock(BLOCK_TEST));
	PH_UpdateShipPhysicsData(ship);
	GO_ShipCloseWithWalls(ship, WALL_LIGHT);
	CD_GenerateShipCollider(ship);
	PH_UpdateShipPhysicsData(ship2);
	GO_ShipCloseWithWalls(ship2, WALL_LIGHT);
	CD_GenerateShipCollider(ship2);
	
	ship->physics.angularVel = 7.8;
	ship->physics.linearVel = (Real2) {0.1, -0.2};
	PH_SetLinearVelocity(bullet, R2_ScalarMult((Real2) {3, -5}, 100));
	GO_LogObjectTree(root, stderr);

	quit = 0;
	startTime = 0;
	timeCoefficient = 1;
	willUpdate = 0;
	stepByStep = 1;
	// Main loop
	GO_PushToTrail(trail, GO_PosToRootSpace(ship, ship->physics.centerOfMass));
	GO_PushToTrail(bulittrail, GO_PosToRootSpace(bullet, PH_GetCenterOfMass(bullet)));
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
			PH_ApplyForce(ship, R2_ScalarMult(R2_Sub(GO_PosToRootSpace(camera, IN_GetMouseCameraPos()), ship->transform.pos), 50), (Real2) {0,0});
			PH_UpdateObjectTree(root, (double) timeCoefficient * TICKS_PER_FRAME / 1000.0);
			if (CD_MayCollide(ship, bullet)) {
				printf("BOOM BAM CRASH BULLIT HIT!!!1!\n");
				GO_ShipAddHole(ship, GO_PosToLocalSpace(ship, GO_PosToRootSpace(bullet, CD_PolygonGetFirstVertex(bullet))));
			}
			GO_PushToTrail(trail, GO_PosToRootSpace(ship, ship->physics.centerOfMass));
			GO_PushToTrail(bulittrail, GO_PosToRootSpace(bullet, PH_GetCenterOfMass(bullet)));
			GR_Render(root);
			PH_ClearAllForces(root);
			willUpdate = 0;
		}
		frameTime = SDL_GetTicks() - startTime;
		if (frameTime < TICKS_PER_FRAME) {
			SDL_Delay(TICKS_PER_FRAME - frameTime);
		}

		printf("FRAME TIME: %d\n", frameTime);
	}
	return 0;
}

