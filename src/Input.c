#include "Input.h"
#include "Transform.h"
#include "List.h"

typedef struct EventObserver EventObserver;
typedef struct KeyStateObserver KeyStateObserver;

struct EventObserver {
	Component *observer;
	void (*handleEvent)(Component *observer, SDL_Event event);
};

List *gEventObservers = NULL;

void IN_UpdateEventHandlers(void);
void IN_UpdateKeyStateHandlers(void);

Real2 IN_GetMouseCameraPos(void)
{
	double xPixelsPerUnit, yPixelsPerUnit;
	int x, y;
	
	xPixelsPerUnit = SCREEN_WIDTH / GR_GetCameraWidth();
	yPixelsPerUnit = SCREEN_HEIGHT / GR_GetCameraHeight();

	SDL_GetMouseState(&x, &y);

	return (Real2) {x / xPixelsPerUnit, y / yPixelsPerUnit};
}

Real2 IN_GetMouseLocalPos(Component *localSpaceTransform)
{
	return TR_PosToLocalSpace(localSpaceTransform, IN_GetMouseRootPos());
}

Real2 IN_GetMouseRootPos(void)
{
	Component *cameraTransform;
	Real2 rootSpaceMousePos;

	cameraTransform = GO_GetComponent(GR_GetMainCamera(), COMP_TRANSFORM);
	rootSpaceMousePos = TR_PosToRootSpace(cameraTransform, IN_GetMouseCameraPos());

	return rootSpaceMousePos;
}

const Uint8 *IN_GetKeyStates(void)
{
	return SDL_GetKeyboardState(NULL);
}

void IN_UpdateInputHandlers()
{
	// Ensure lists are initialized
	if (gEventObservers == NULL) gEventObservers = List_Nil();

	IN_UpdateEventHandlers();
}

void IN_UpdateEventHandlers(void)
{
	List *evlist;
	SDL_Event event;
	EventObserver* currentObserver;

	while (SDL_PollEvent(&event) != 0) {
		evlist = gEventObservers;
		LOOP_OVER(evlist) {
			currentObserver = List_Head(evlist);
			(currentObserver->handleEvent)(currentObserver->observer, event);
		}
	}
}

void IN_AddEventHandler(Component *observer, void (*handler)(Component *, SDL_Event)) {
	EventObserver *eventObserver = malloc(sizeof(EventObserver));
	eventObserver->observer = observer;
	eventObserver->handleEvent = handler;

	List_Append(gEventObservers, eventObserver);
}

// TODO: Make sure ALL relevant handlers are removed (now not happening)
void IN_RemoveEventHandler(Component *observer) {
	List *evlist;
	int i;
	EventObserver *current;

	evlist = gEventObservers;
	i = 0;
	LOOP_OVER(evlist) {
		if ((current = ((EventObserver *)List_Head(evlist)))->observer == observer) {
			List_Delete(gEventObservers, i);
			free(current);
			break;
		} 
		i++;
		
	}
}
