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

typedef struct Intersection Intersection;

struct Intersection {
	Real2 pos;
	Real2 normal;
};

int _CO_IsOverBlock(Object *ship, int x, int y);
int _CO_IsOverBlockDir(Object *ship, int x, int y, int dir);
void _CO_UpdateXY(int *x, int *y, int dir);

void CO_GenerateShipCollider(Object *ship)
{
	int width, height, i, j, x, y, prev, startX, startY;
	Polygon *polygon;

	startX = startY = prev = -1;
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

void CO_ShipHandleCollision(Object *ship, Object *other)
{
	switch (other->type) {
		case OBJ_PROJECTILE:
			break;
			
	}
}

//TODO: This procedure contains a silly amount of placeholder hacks
void CO_CheckCollision(Object *obj0, Object *obj1)
{
	int type0, type1;

	type0 = obj0->type;
	type1 = obj1->type;
	
	// No collision in current frame
	//TODO: Continuous collision should replace this shitty test
	if (!CO_MayCollide(obj0, obj1)) {
		return;
	}

	if ( type0 == OBJ_PROJECTILE && type1 == OBJ_SHIP
	  || type0 == OBJ_SHIP && type1 == OBJ_PROJECTILE) {
		//TODO: CCD HERE
	}
}

void CO_GenerateProjectileCollider(Object *projectile)
{
	double size;	
	Polygon *polygon;

	polygon = PG_CreateEmpty();
	projectile->collider.collider = polygon;
	size = ((Projectile *)projectile->obj)->size;
	PG_AppendVertex(polygon, (Real2) {size/2, size/2});
}

Interval CO_ProjectOnAxis(Object *obj, Real2 axis)
{
	Interval interval;
	List *vertex; 
	Real2 axisNorm, rootVertex; 
	double projection;

	axisNorm = R2_Norm(axis);
	vertex = ((Polygon *) obj->collider.collider)->vertices;
	if (vertex == NULL) {
		interval.start = 0;
		interval.end = 0;
	} else {
		// Initialize interval to first element
		rootVertex = GO_GetRootPositionFrom(obj, *((Real2 *)List_Head(vertex)));
		projection = R2_ProjectOnUnit(rootVertex, axisNorm);
		interval.start = projection;
		interval.end = projection;
		vertex = List_Tail(vertex);
		// Run through rest of vertices
		while (vertex != NULL) {
			rootVertex = GO_GetRootPositionFrom(obj, *((Real2 *)List_Head(vertex)));
			projection = R2_ProjectOnUnit(rootVertex, axisNorm);
			interval.start = projection < interval.start ? projection : interval.start;
			interval.end = projection > interval.end ? projection : interval.end;
			vertex = List_Tail(vertex);
		}
	}

	return interval;
}

int CO_MayCollide(Object *obj1, Object *obj2)
{
	int mayCollide;
	Interval intervalObj1, intervalObj2;
	List *vertexList; 
	Real2 currVertex, nextVertex, axis; 

	mayCollide = 1;
	vertexList = ((Polygon *) obj1->collider.collider)->vertices;
	if (vertexList == NULL) {
		fprintf(stderr, "CO_MayCollide: obj1's collider is NULL\n");
		exit(1);
	}
	nextVertex = GO_GetRootPositionFrom(obj1, *((Real2 *)List_Head(vertexList)));
	while (List_HasTail(vertexList)) {
		currVertex = nextVertex;
		vertexList = List_Tail(vertexList);
		nextVertex = GO_GetRootPositionFrom(obj1, *((Real2 *)List_Head(vertexList)));
		axis = R2_Norm(R2_Normal(R2_Sub(nextVertex, currVertex)));
		intervalObj1 = CO_ProjectOnAxis(obj1, axis);
		intervalObj2 = CO_ProjectOnAxis(obj2, axis);
		mayCollide = mayCollide && IN_IsIntersecting(&intervalObj1, &intervalObj2);
	}
	return mayCollide;
}

Real2 CO_PolygonGetFirstVertex(Object *obj)
{
	if (obj->collider.type != COLL_POLYGON) {
		fprintf(stderr, "Error: %s does not have a polygon collider\n", obj->name);
		exit(1);
	}
	return PG_GetFirstVertex(obj->collider.collider);
}

// ---------------------
// Local procedures
// Is vertex over block
int _CO_IsOverBlock(Object *ship, int x, int y)
{       
	int overBlock, width, height;

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
