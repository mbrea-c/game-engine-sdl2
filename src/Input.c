#include "Input.h"

Real2 IN_GetMouseCameraPos(void)
{
	double xPixelsPerUnit, yPixelsPerUnit;
	int x, y;
	
	xPixelsPerUnit = SCREEN_WIDTH / GR_GetCameraWidth();
	yPixelsPerUnit = SCREEN_HEIGHT / GR_GetCameraHeight();

	SDL_GetMouseState(&x, &y);

	return (Real2) {x / xPixelsPerUnit, y / yPixelsPerUnit};
}
