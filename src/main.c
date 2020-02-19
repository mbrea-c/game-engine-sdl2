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

#define FPS_CAP 60
#define TICKS_PER_FRAME 1000 / FPS_CAP

int main(int argc, char **argv)
{
	int quit, willUpdate;
	double timeCoefficient;
	Uint32 startTime, frameTime;
	SDL_Event event;
	GR_Init();
	Object *root = GO_CreateObject(OBJ_WORLD_NODE, "top", 0, 0, 0, NULL, NULL);
	Object *camera = GO_CreateCamera(0, 0, "camerita", root, 30);
	Object *ship = GO_CreateEmptyShip("shipi", 7, 7, 0, root, 10, 15); 
	Object *trail = GO_CreateTrail("shipi trail", root, 1000);
	Object *bullet = GO_CreateProjectile("bullit", 0, 20, root, PROJ_BULLET, 0.3);
	GO_ShipSetBlock(ship, 0,0, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship, 0,1, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship, 1,0, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship, 1,1, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship, 2,1, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship, 3,1, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship, 4,1, GO_CreateUnwalledBlock(BLOCK_TEST));
	GO_ShipSetBlock(ship, 4,0, GO_CreateUnwalledBlock(BLOCK_TEST));
	PH_UpdateShipPhysicsData(ship);
	GO_ShipCloseWithWalls(ship, WALL_LIGHT);
	CD_GenerateShipCollider(ship);
	
	ship->physics.angularVel = 7.8;
	ship->physics.linearVel = (Real2) {0.1, -0.2};
	PH_SetLinearVelocity(bullet, R2_ScalarMult((Real2) {3, -5}, 10));
	printf("MASS m%f\n", ship->physics.mass);
	printf("COM  x%fy%f\n", ship->physics.centerOfMass.x, ship->physics.centerOfMass.y);
	GO_LogObjectTree(root, stderr);

	quit = 0;
	startTime = 0;
	timeCoefficient = 1;
	willUpdate = 0;
	// Main loop
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
				}
			}
		}
		if (willUpdate) {
			GO_PushToTrail(trail, GO_PosToRootSpace(ship, ship->physics.centerOfMass));
			PH_UpdateObjectTree(root, (double) timeCoefficient * TICKS_PER_FRAME / 1000.0);
			if (CD_MayCollide(ship, bullet)) {
				printf("BOOM BAM CRASH BULLIT HIT!!!1!\n");
				GO_ShipAddHole(ship, GO_PosToLocalSpace(ship, GO_PosToRootSpace(bullet, CD_PolygonGetFirstVertex(bullet))));
			}
		}
		GR_Render(root);
		frameTime = SDL_GetTicks() - startTime;
		/*printf("FRAME TICKS: %d\n", frameTime);*/
		if (frameTime < TICKS_PER_FRAME) {
			SDL_Delay(TICKS_PER_FRAME - frameTime);
		}
	}
	return 0;
}

