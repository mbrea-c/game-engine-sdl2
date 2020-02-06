#ifndef POLYGON_H
#define POLYGON_H

#include "Real2.h"
#include "List.h"

// TODO: Implement all dis
typedef struct Polygon Polygon;

struct Polygon {
	List *vertices;
};

Polygon *PG_CreateEmpty(void);
void PG_AppendVertex(Polygon *polygon, Real2 vertex);
void PG_RemoveVertex(Polygon *polygon, int i);

#endif //POLYGON_H
