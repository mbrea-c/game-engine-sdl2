#include "LTexture.h"

// TODO: Error checking
LTexture *LT_LoadFromFile(char *path, SDL_Renderer *renderer)
{
	LTexture *newLTexture = malloc(sizeof(LTexture));
	SDL_Texture *newTexture = NULL;
	SDL_Surface *loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL) {
		fprintf(stderr, "SDL could not load media! SDL_image error: %s\n", IMG_GetError());
		exit(1);
	}
	newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
	if (newTexture == NULL) {
		fprintf(stderr, "Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
		exit(1);
	}
	newLTexture->texture = newTexture;
	newLTexture->renderer = renderer;
	newLTexture->height = loadedSurface->h;
	newLTexture->width = loadedSurface->w;

	SDL_FreeSurface(loadedSurface);
	return newLTexture;
}

void LT_DestroyTexture(LTexture *lTexture)
{
	if (lTexture->texture != NULL) 
		SDL_DestroyTexture(lTexture->texture);
	free(lTexture);
}

void LT_Render(LTexture *lTexture, int x, int y, SDL_Rect *clip, double angle,
		SDL_Point *center, SDL_RendererFlip flip)
{
	SDL_Rect renderViewport = { x, y, lTexture->width, lTexture->height };

	// Set clip rendering dimensions
	if (clip != NULL) {
		renderViewport.w = clip->w;
		renderViewport.h = clip->h;
	}

	SDL_RenderCopyEx(lTexture->renderer, lTexture->texture, clip, &renderViewport, angle, center, flip);
}

void LT_RenderStretch(LTexture *lTexture, int x, int y, int w, int h, SDL_Rect *clip, double angle,
		SDL_Point *center, SDL_RendererFlip flip)
{
	SDL_Rect renderViewport = { x, y, w, h };

	SDL_RenderCopyEx(lTexture->renderer, lTexture->texture, clip, &renderViewport, angle, center, flip);
}

void LT_RenderRect(LTexture *lTexture, SDL_Rect *target, SDL_Rect *clip, double angle,
		SDL_Point *center, SDL_RendererFlip flip)
{
	SDL_RenderCopyEx(lTexture->renderer, lTexture->texture, clip, target, angle, center, flip);
}
