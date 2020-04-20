#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include "LTexture.h"
#include "GameObject.h"
#include "Assets.h"
#include "Polygon.h"
#include "Real2.h"
#include "List.h"

// Screen dimension constants
#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

// Debug drawing
#define DRAW_COLLIDERS 1
#define DRAW_TORQUES 1
#define DRAW_FORCES 1
#define DRAW_GRID 1


void GR_Init(Object *camera);
void GR_Render(Object *root);
SDL_Renderer *GR_GetMainRenderer(void);
double GR_GetCameraHeight();
double GR_GetCameraWidth();
void GR_DrawForce(Object *obj, Real2 force, Real2 localPos);


#endif //GRAPHICS_H
