#include "Collider.h"


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

int _CO_IsOverBlock(Object *ship, int x, int y);
int _CO_IsOverBlockDir(Object *ship, int x, int y, int dir);
void _CO_UpdateXY(int *x, int *y, int dir);

void CO_GenerateShipCollider(Object *ship)
{
	int width, height, i, j, x, y, prev, startX, startY, done;
	Polygon *polygon;

	startX = startY = prev = -1;
	done = 0;
	width = GO_ShipGetWidth(ship);
	height = GO_ShipGetHeight(ship);
	polygon = PG_CreateEmpty();
	ship->collider.collider = polygon;

	// Find first vertex
	for (y = 0; y <= height; y++) {
		for (x = 0; x <= width; x++) {
			if (_CO_IsOverBlock(ship, x, y)) {
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
		if (_CO_IsOverBlockDir(ship, x, y, i)) break;
	}

	for (j = 0; j < DIR_COUNT; i = (i - 1) % DIR_COUNT, j++) {
		if (!_CO_IsOverBlockDir(ship, x, y, i)) break;
	}

	_CO_UpdateXY(&x, &y, (i+1) % DIR_COUNT);
	prev = (i+3) % DIR_COUNT;

	// Find rest of vertices
	while (1) {
		if (x == startX && y == startY) {
			break;
		}

		for (i = (prev + 1) % DIR_COUNT, j = 0; j < DIR_COUNT; i = (i + 1) % DIR_COUNT, j++) {
			if (_CO_IsOverBlockDir(ship, x, y, i)) break;
		}

		// Only add a vertex if there is a change of direction,
		// this minimizes the number of vertices in the final
		// polygon
		if ((i + 2) % DIR_COUNT != prev) {
			PG_AppendVertex(polygon, (Real2) {x, y});
			prev = (i+2) % DIR_COUNT;
		}
		_CO_UpdateXY(&x, &y, i);
	}

}

// Is vertex over block
int _CO_IsOverBlock(Object *ship, int x, int y)
{       
	int overBlock, width, height, i;

	width = GO_ShipGetWidth(ship);
	height = GO_ShipGetHeight(ship);
	overBlock = 0;

	if (x < 0 || x > width || y < 0 || y > height) {
		return 0;
	}
	if (x > 0 && y > 0) {
		overBlock = overBlock || (GO_ShipGetBlock(ship, x - 1, y - 1)->type != BLOCK_EMPTY);
	}
	if (x > 0 && y < height) {
		overBlock = overBlock || (GO_ShipGetBlock(ship, x - 1, y)->type != BLOCK_EMPTY);
	}
	if (x < width && y > 0) {
		overBlock = overBlock || (GO_ShipGetBlock(ship, x, y - 1)->type != BLOCK_EMPTY);
	}
	if (x < width && y < height) {
		overBlock = overBlock || (GO_ShipGetBlock(ship, x, y)->type != BLOCK_EMPTY);
	}
	return overBlock;

}

// Is vertex at given direction over block
int _CO_IsOverBlockDir(Object *ship, int x, int y, int dir)
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
	return _CO_IsOverBlock(ship, x, y);
}

void _CO_UpdateXY(int *x, int *y, int dir)
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
