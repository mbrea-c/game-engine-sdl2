#include "Polygon.h"
#include "Debug.h"

//TODO: This is only in .h for debuggin' boi

Polygon *PG_CreateEmpty(void)
{
	Polygon *p = malloc(sizeof(Polygon));
	p->vertices = List_Nil();
	return p;
}

void PG_AppendVertex(Polygon *polygon, Real2 vertex)
{
	Real2 *v = malloc(sizeof(Real2));
	*v = vertex;
	List_Append(polygon->vertices, v);
}

Real2 PG_GetFirstVertex(Polygon *polygon)
{
	return *((Real2 *) List_Head(polygon->vertices));
}

// Polygon destructor
void PG_Destructor(Polygon *polygon)
{
	List *verticesList;

	verticesList = polygon->vertices;
	LOOP_OVER(verticesList) {
		free(List_Head(verticesList));
	}
	List_Destructor(polygon->vertices);
	free(polygon);
}

