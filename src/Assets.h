#ifndef ASSETS_H
#define ASSETS_H

#include "LTexture.h"
#include "SDL2/SDL.h"
#include "Typedefs.h"

Material *gMaterials[BLOCK_COUNT];
WallType *gWallTypes[WALL_COUNT];

void AS_LoadAll(SDL_Renderer *renderer);

#endif //ASSETS_H
