#include "Polygon.h"

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

