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

// Screen dimension constants
#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720


void GR_Init();
void GR_Render(Object *root);
SDL_Renderer *GR_GetMainRenderer(void);


#endif //GRAPHICS_H
