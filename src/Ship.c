#include "Ship.h"
#include "Component.h"
#include "Assets.h"
#include "Physics.h"
#include "Collider.h"
#include "Debug.h"

// Directions
//     0  
//   3 + 1
//     2  
enum edgeDir {
	UP,
	RIGHT,
	DOWN,
	LEFT,
	DIR_COUNT,
};

int shipDependencies[] = { COMP_TRANSFORM, COMP_PHYSICS, COMP_COLLIDER };

// DECLARATIONS (Local procedures)
void SH_InitBlock(Component *ship, int x, int y, Block block);
// Helpers for collider generation
int SH_IsVertexOverBlock(Component *ship, int x, int y);
int SH_IsVertexOverBlockDir(Component *ship, int x, int y, int dir);
void SH_UpdateXYByDir(int *x, int *y, int dir);

// Basic
Component *SH_CreateShipEmpty(int width, int height)
{
	int i, j;
	unsigned int k;
	ShipData *shipData; 
	Component *shipComponent;
	List *dependenciesList;

	printf("DEBUG: Creating ship size %dx%d\n", width, height);
	// Create dependencies list
	dependenciesList = List_Nil();
	for (k = 0; k < sizeof(shipDependencies)/sizeof(int); k++) {
		List_Append(dependenciesList, &shipDependencies[k]);
	}

	shipData = malloc(sizeof(ShipData));
	shipData->width = width;
	shipData->height = height;
	shipData->blocks = malloc(width * height * sizeof(Block));
	shipData->holes = List_Nil();
	shipComponent = CM_CreateComponent(COMP_SHIP, shipData, &SH_Destructor, &SH_Mount, dependenciesList);

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			SH_InitBlock(shipComponent, i, j, SH_CreateUnwalledBlock(BLOCK_EMPTY));
		}
	}

	printf("DEBUG: Created ship\n");
	return shipComponent;
}

void SH_Mount(Component *ship)
{
	// Initialize physics data
	SH_UpdatePhysicsData(ship);
	// Initialize ship collider
	SH_UpdateCollider(ship);

}

void SH_Destructor(void *shipData)
{
	free(((ShipData *)shipData)->blocks);
	free(shipData);
	//TODO: vvvvvSEEvvvvvv
	//TODO: MEMLEAK: Need to deallocate List, each block and each hole
	//TODO: ^^^^^SEE^^^^^^
}

// Getters
int SH_GetHeight(Component *ship)
{
	return ((ShipData *)ship->componentData)->height;
}

int SH_GetWidth(Component *ship)
{
	return ((ShipData *)ship->componentData)->width;
}

Block *SH_GetBlocks(Component *ship)
{
	return ((ShipData *)ship->componentData)->blocks;
}

List *SH_GetHoles(Component *ship)
{
	return ((ShipData *)ship->componentData)->holes;
}

Block *SH_GetBlock(Component *ship, int x, int y)
{
	int width, height;
	width = SH_GetWidth(ship);
	height = SH_GetHeight(ship);
	Block *blocks = SH_GetBlocks(ship); 
	// Input bound checking
	if (x < 0 || x >= width) {
		fprintf(stderr, "GO_ShipGetBlock: Array x coordinate out of bounds!\n");
		exit(1);
	} else if (y < 0 || y >= height) {
		fprintf(stderr, "GO_ShipGetBlock: Array y coordinate out of bounds!\n");
		exit(1);
	}

	return (blocks + y * width + x);
}

// Setters

void SH_SetBlock(Component *ship, int x, int y, Block block)
{
	SH_InitBlock(ship, x, y, block);
	SH_UpdatePhysicsData(ship);
	// TODO: This needs to change when we fix the relevant methods
	SH_UpdateCollider(ship);
}

void SH_SetBlockWall(Component *ship, int x, int y, int wall, int wallType)
{
	SH_GetBlock(ship, x, y)->walls[wall] = gWallTypes[wallType];
}


// Ship specific procedures

void SH_CloseWithWalls(Component *ship, int wallType)
{
	int width, height, x, y;
	Block *block;

	width = SH_GetWidth(ship);
	height = SH_GetHeight(ship);

	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {
			block = SH_GetBlock(ship, x, y);	
			if (block->type != BLOCK_EMPTY) {
				if (x <= 0 || SH_GetBlock(ship, x-1, y)->type == BLOCK_EMPTY) {
					block->walls[LEFT_WALL] = gWallTypes[wallType];
				}
				if (y <= 0 || SH_GetBlock(ship, x, y-1)->type == BLOCK_EMPTY) {
					block->walls[TOP_WALL] = gWallTypes[wallType];
				}
				if (x >= width - 1 || SH_GetBlock(ship, x+1, y)->type == BLOCK_EMPTY) {
					block->walls[RIGHT_WALL] = gWallTypes[wallType];
				}
				if (y >= height - 1 || SH_GetBlock(ship, x, y+1)->type == BLOCK_EMPTY) {
					block->walls[BOTTOM_WALL] = gWallTypes[wallType];
				}
			}
		}
	}
}

void SH_AddHole(Component *ship, Real2 damagePos)
{
	ShipHole *hole;
	int hx, hy;

	hx = (int) damagePos.x;
	hy = (int) damagePos.y;

	hole = malloc(sizeof(ShipHole));
	hole->pos = damagePos;
	// Find two closest block to damage position
	// block it lies on
	hole->xBlock1 = hx;
	hole->yBlock1 = hy;
	// second closest block
	hole->xBlock2 = damagePos.x - hx < hx + 1 - damagePos.x ? hx - 1 : hx + 1;
	hole->yBlock2 = damagePos.y - hy < hy + 1 - damagePos.y ? hy - 1 : hy + 1;

	// Add hole info to ship hole list
	List_Append(SH_GetHoles(ship), hole);
}

void SH_UpdateMass(Component *ship)
{
	int x, y;
	double mass;
	Component *shipPhysics;

	shipPhysics = GO_GetComponent(GO_GetComponentOwner(ship), COMP_PHYSICS);
	mass = 0;
	for (y = 0; y < SH_GetHeight(ship); y++) {
		for (x = 0; x < SH_GetWidth(ship); x++) {
			mass += SH_GetBlock(ship, x, y)->material->mass;
		}
	}
	PH_SetMass(shipPhysics, mass);
}

void SH_UpdateCOM(Component *ship)
{
	Real2 com;
	int x, y;
	double shipMass;
	Component *shipPhysics;

	shipPhysics = GO_GetComponent(GO_GetComponentOwner(ship), COMP_PHYSICS);
	com = (Real2) { 0, 0 };
	for (y = 0; y < SH_GetHeight(ship); y++) {
		for (x = 0; x < SH_GetWidth(ship); x++) {
			com.x += SH_GetBlock(ship, x, y)->material->mass * (x + 0.5);
			com.y += SH_GetBlock(ship, x, y)->material->mass * (y + 0.5);
		}
	}
	if ((shipMass = PH_GetMass(shipPhysics)) != 0) {
		com.x /= shipMass;
		com.y /= shipMass;
	}
	PH_SetCenterOfMass(shipPhysics, com);
}

void SH_UpdateMOI(Component *ship)
{
	int x, y;
	double moi, distToCOM;
	Real2 com;
	Component *shipPhysics;

	shipPhysics = GO_GetComponent(GO_GetComponentOwner(ship), COMP_PHYSICS);
	moi = 0;
	com = PH_GetCenterOfMass(shipPhysics);
	for (y = 0; y < SH_GetHeight(ship); y++) {
		for (x = 0; x < SH_GetWidth(ship); x++) {
			distToCOM = R2_DistSq(com, (Real2) { x + 0.5, y + 0.5 }); // Get distance squared
			moi += SH_GetBlock(ship, x, y)->material->mass * (distToCOM);
		}
	}
	PH_SetMomentOfInertia(shipPhysics, moi);
}

void SH_UpdatePhysicsData(Component *ship)
{
	SH_UpdateMass(ship);
	SH_UpdateCOM(ship);
	SH_UpdateMOI(ship);
}

void SH_UpdateCollider(Component *ship)
{
	int width, height, i, j, x, y, prev, startX, startY;
	Polygon *polygon;
	Component *collider;

	collider = GO_GetComponent(GO_GetComponentOwner(ship), COMP_COLLIDER);
	CD_ResetCollider(collider, COLL_POLYGON);
	polygon = CD_GetCollider(collider);
	startX = startY = prev = -1;
	width = SH_GetWidth(ship);
	height = SH_GetHeight(ship);

	// Find first vertex
	for (y = 0; y <= height; y++) {
		for (x = 0; x <= width; x++) {
			if (SH_IsVertexOverBlock(ship, x, y)) {
				startX = x;
				startY = y;
				break;
			}
		}
		if (startX != -1) {
			break;
		}
	}

	// If no vertex found
	if (startX == -1) {
		return;
	}

	// First vertex found
	PG_AppendVertex(polygon, (Real2) {x, y});

	// Find direction of second vertex, must be done before while loop
	// because procedure is different, we don't know which vertex we are
	// coming from
	for (i = 0; i < DIR_COUNT; i++) {
		if (SH_IsVertexOverBlockDir(ship, x, y, i)) break;
	}

	for (j = 0; j < DIR_COUNT; i = (i - 1) % DIR_COUNT, j++) {
		if (!SH_IsVertexOverBlockDir(ship, x, y, i)) break;
	}

	SH_UpdateXYByDir(&x, &y, (i+1) % DIR_COUNT);
	prev = (i+3) % DIR_COUNT;

	// Find rest of vertices
	while (1) {
		if (x == startX && y == startY) {
			break;
		}

		for (i = (prev + 1) % DIR_COUNT, j = 0; j < DIR_COUNT; i = (i + 1) % DIR_COUNT, j++) {
			if (SH_IsVertexOverBlockDir(ship, x, y, i)) break;
		}

		// Only add a vertex if there is a change of direction,
		// this minimizes the number of vertices in the final
		// polygon
		if ((i + 2) % DIR_COUNT != prev) {
			PG_AppendVertex(polygon, (Real2) {x, y});
			prev = (i+2) % DIR_COUNT;
		}
		SH_UpdateXYByDir(&x, &y, i);
	}
}

// Block procedures

Block SH_CreateUnwalledBlock(int type)
{
	int k;
	Block newBlock;
	newBlock.type = type;
	newBlock.material = gMaterials[type];
	for (k = 0; k < 4; k++) {
		newBlock.walls[k] = gWallTypes[WALL_EMPTY];
	}
	return newBlock;
}

// Local procedures
void SH_InitBlock(Component *ship, int x, int y, Block block)
{
	*(SH_GetBlock(ship, x, y)) = block;
}

// Is vertex over block
int SH_IsVertexOverBlock(Component *ship, int x, int y)
{       
	int overBlock, width, height;

	width = SH_GetWidth(ship);
	height = SH_GetHeight(ship);
	overBlock = 0;

	if (x < 0 || x > width || y < 0 || y > height) {
		return 0;
	}
	if (x > 0 && y > 0) {
		overBlock = overBlock || (SH_GetBlock(ship, x - 1, y - 1)->type != BLOCK_EMPTY);
	}
	if (x > 0 && y < height) {
		overBlock = overBlock || (SH_GetBlock(ship, x - 1, y)->type != BLOCK_EMPTY);
	}
	if (x < width && y > 0) {
		overBlock = overBlock || (SH_GetBlock(ship, x, y - 1)->type != BLOCK_EMPTY);
	}
	if (x < width && y < height) {
		overBlock = overBlock || (SH_GetBlock(ship, x, y)->type != BLOCK_EMPTY);
	}
	return overBlock;

}

// Is vertex at given direction over block
int SH_IsVertexOverBlockDir(Component *ship, int x, int y, int dir)
{
	switch (dir) {
		case UP:
			y--;
			break;
		case RIGHT:
			x++;
			break;
		case DOWN:
			y++;
			break;
		case LEFT:
			x--;
			break;
	}
	return SH_IsVertexOverBlock(ship, x, y);
}

void SH_UpdateXYByDir(int *x, int *y, int dir)
{
	switch (dir) {
		case UP:
			(*y)--;
			break;
		case RIGHT:
			(*x)++;
			break;
		case DOWN:
			(*y)++;
			break;
		case LEFT:
			(*x)--;
			break;
	}
}

