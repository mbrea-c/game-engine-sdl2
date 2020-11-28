#include "Engine.h"
#include "GameObject.h"
#include "Trail.h"
#include "Graphics.h"
#include "Physics.h"
#include "Collider.h"

void EN_RunEngine(Object *sceneRoot)
{
	int quit, willUpdate, stepByStep;
	double timeCoefficient;
	Uint32 startTime, frameTime;
	SDL_Event event;

	
	GO_LogObjectTree(sceneRoot, stderr);

	quit = 0;
	startTime = 0;
	timeCoefficient = 1;
	willUpdate = 1;
	stepByStep = 1;
	// Main loop

	while (!quit) {
		startTime = SDL_GetTicks();
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quit = 1;
			} else if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						quit = 1;
						break;
					case SDLK_SPACE:
						willUpdate = 1;
						break;
					case SDLK_b:
						stepByStep = !stepByStep;
						break;
				}
			}
		}
			
		if (willUpdate || !stepByStep) {
			// call update on each component
			GO_UpdateMountedComponents();
			// perform physiscs update
			PH_UpdateAllObjects((double) timeCoefficient * TICKS_PER_FRAME / 1000.0);
			// update collisions
			CD_ComputeCollisions();
			// update graphics
			GR_Render(sceneRoot);
			// clean up forces in physics objects
			PH_ClearAllForces(sceneRoot);
			willUpdate = 0;
		}

		frameTime = SDL_GetTicks() - startTime;
		if (frameTime < TICKS_PER_FRAME) {
			SDL_Delay(TICKS_PER_FRAME - frameTime);
		}

		printf("FRAME TIME: %d\n", frameTime);
	}
}
