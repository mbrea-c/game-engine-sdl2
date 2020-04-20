#include "Trail.h"
#include "Component.h"
#include "GameObject.h"
#include "Transform.h"

int trailDependencies[] = { COMP_TRANSFORM };

// Local declarations
void TL_SetPoint(Component *trail, int index, Real2 point);
void TL_IncrementNext(Component *trail);

// Basic
Component *TL_Create(int length, RGBA color)
{
	int i;
	Component *trail;
	TrailData *trailData;
	List *dependenciesList;

	// Create dependencies list
	dependenciesList = List_Nil();
	for (i = 0; i < sizeof(trailDependencies)/sizeof(int); i++) {
		List_Append(dependenciesList, &trailDependencies[i]);
	}

	trailData = malloc(sizeof(TrailData));
	trailData->length = length;
	trailData->points = calloc(length, sizeof(Real2));
	trailData->next = 0;
	trailData->color = color;
	trail = CM_CreateComponent(COMP_TRAIL, trailData, &TL_Destructor, &TL_Mount, dependenciesList);
	return trail;
}
void TL_Mount(Component *trail)
{
	int i;
	Component *transform;

	transform = GO_GetComponentFromOwner(trail, COMP_TRANSFORM);
	for (i = 0; i < TL_GetLength(trail); i++) {
		TL_SetPoint(trail, i, TR_GetPos(transform));
	}
}

void TL_Destructor(void *trailData)
{
	free(((TrailData *)trailData)->points);
	free(trailData);
}

// Getters
int TL_GetLength(Component *trail)
{
	return ((TrailData *)trail->componentData)->length;
}

int TL_GetNext(Component *trail)
{
	return ((TrailData *)trail->componentData)->next;
}

Real2 *TL_GetPoints(Component *trail)
{
	return ((TrailData *)trail->componentData)->points;
}

Real2 TL_GetPoint(Component *trail, int index)
{
	return ((TrailData *)trail->componentData)->points[index];
}

RGBA TL_GetColor(Component *trail)
{
	return ((TrailData *)trail->componentData)->color;
}

// Setters
void TL_SetColor(Component *trail, RGBA color)
{
	((TrailData *)trail->componentData)->color = color;
}

// Trail specific procedures
void TL_PushToTrail(Component *trail)
{
	int next;
	Component *transform;

	transform = GO_GetComponentFromOwner(trail, COMP_TRANSFORM);
	next = TL_GetNext(trail);
	TL_SetPoint(trail, next, TR_PosToRootSpace(transform, TR_GetPos(transform)));
	TL_IncrementNext(trail);
}

// Local procedure definitions
void TL_SetPoint(Component *trail, int index, Real2 point)
{
	TL_GetPoints(trail)[index] = point;
}

void TL_IncrementNext(Component *trail)
{
	int next;
	next = (TL_GetNext(trail) + 1) % TL_GetLength(trail);
	((TrailData *)trail->componentData)->next = next;
}
