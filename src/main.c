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
#include "Collider.h"

#define FPS_CAP 60
#define TICKS_PER_FRAME 1000 / FPS_CAP

int main(int argc, char **argv)
{
	int quit;
	Uint32 startTime, frameTime;
	SDL_Event event;
	GR_Init();
	Object *root = GO_CreateObject(OBJ_WORLD_NODE, "top", 0, 0, 0, NULL, NULL);
	Object *camera = GO_CreateCamera(0, 0, "camerita", root, 30);
	Object *ship = GO_CreateEmptyShip("shipi", 7, 7, 0, root, 10, 15); 
	Object *trail = GO_CreateTrail("shipi trail", root, 100);
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
	CO_GenerateShipCollider(ship);
	
	// DELETE
	int x,y;
	for (y = 0; y < 15; y++) {
		for (x = 0; x < 10; x++) {
			printf("%d ", GO_ShipGetBlock(ship, x, y)->type);
		}
		printf("\n");
	}
	// DELETE END
	
	ship->physics.angularVel = 7.8;
	ship->physics.linearVel = (Real2) {0.1, -0.2};
	printf("MASS m%f\n", ship->physics.mass);
	printf("COM  x%fy%f\n", ship->physics.centerOfMass.x, ship->physics.centerOfMass.y);
	GO_LogObjectTree(root, stderr);

	quit = 0;
	startTime = 0;
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
				}
			}
		}
		GO_PushToTrail(trail, GO_GetRootPositionFrom(ship, ship->physics.centerOfMass));
		PH_UpdateObjectTree(root, (double) TICKS_PER_FRAME / 1000.0);
		GR_Render(root);
		frameTime = SDL_GetTicks() - startTime;
		/*printf("FRAME TICKS: %d\n", frameTime);*/
		if (frameTime < TICKS_PER_FRAME) {
			SDL_Delay(TICKS_PER_FRAME - frameTime);
		}
	}
	return 0;
}

