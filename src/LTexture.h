#ifndef LTEXTURE_H
#define LTEXTURE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

typedef struct {
	SDL_Texture *texture;
	SDL_Renderer *renderer;
	int height;
	int width;
} LTexture;

LTexture *LT_LoadFromFile(char *path, SDL_Renderer *renderer);
void LT_DestroyTexture(LTexture *lTexture);
void LT_Render(LTexture *lTexture, int x, int y, SDL_Rect *clip, double angle,
		SDL_Point *center, SDL_RendererFlip flip);
void LT_RenderStretch(LTexture *lTexture, int x, int y, int w, int h, SDL_Rect *clip, double angle,
		SDL_Point *center, SDL_RendererFlip flip);
void LT_RenderRect(LTexture *lTexture, SDL_Rect *target, SDL_Rect *clip, double angle,
		SDL_Point *center, SDL_RendererFlip flip);

#endif //LTEXTURE_H
