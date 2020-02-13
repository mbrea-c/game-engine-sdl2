#include "Assets.h"

void AS_LoadAll(SDL_Renderer *renderer)
{
	gMaterials[BLOCK_TEST] = malloc(sizeof(Material));
	*(gMaterials[BLOCK_TEST]) = (Material) {
		BLOCK_TEST,
		LT_LoadFromFile("assets/FLOOR00.png", renderer),
		100
	};
	gMaterials[BLOCK_EMPTY] = malloc(sizeof(Material));
	*(gMaterials[BLOCK_EMPTY]) = (Material) {
		BLOCK_EMPTY,
		LT_LoadFromFile("assets/FLOOR01.png", renderer),
		0
	};
	gWallTypes[WALL_LIGHT] = malloc(sizeof(WallType));
	*(gWallTypes[WALL_LIGHT]) = (WallType) {
		WALL_LIGHT,
		LT_LoadFromFile("assets/WALL_00.png", renderer),
		50
	};
	gWallTypes[WALL_EMPTY] = malloc(sizeof(WallType));
	*(gWallTypes[WALL_EMPTY]) = (WallType) {
		WALL_EMPTY,
		LT_LoadFromFile("assets/EMPTY00.png", renderer),
		0,
	};
	gProjectileTypes[PROJ_BULLET] = malloc(sizeof(ProjectileType));
	*(gProjectileTypes[PROJ_BULLET]) = (ProjectileType) {
		PROJ_BULLET,
		LT_LoadFromFile("assets/BULLET.png", renderer),
		50,
	};
}
