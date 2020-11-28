#include "Collider.h"
#include "Component.h"
#include "Ship.h"
#include "Circle.h"
#include "Segment.h"
#include "Transform.h"
#include "Debug.h"

List *gColliderList = NULL;

int colliderDependencies[] = { COMP_TRANSFORM };

// Local procedure declarations
void CD_Mount(Component *collider);
void CD_Update(Component *collider);
void CD_DestroyData(void *colliderData);
void CD_Destructor(void *colliderData);
void CD_CollidePair(Component *collider1, Component *collider2);
void CD_CollideCircleCircle(Component *circle1, Component *circle2);
void CD_CollideCircleSegment(Component *circle, Component *segment);
void CD_CollideCirclePolygon(Component *circle, Component *polygon);
void CD_CollideSegmentSegment(Component *segment1, Component *segment2);
void CD_CollideSegmentPolygon(Component *segment, Component *polygon);
void CD_CollidePolygonPolygon(Component *polygon1, Component *polygon2);

// Basic
Component *CD_CreateCollider(int type)
{
	unsigned int i;
	Component *collider;
	ColliderData *colliderData;
	List *dependenciesList;

	dependenciesList = List_Nil();
	for (i = 0; i < sizeof(colliderDependencies)/sizeof(int); i++) {
		List_Append(dependenciesList, &colliderDependencies[i]);
	}
	colliderData = malloc(sizeof(ColliderData));
	colliderData->type = type;
	switch (type) {
		case COLL_POLYGON:
			colliderData->collider = PG_CreateEmpty();
			break;
		case COLL_CIRCLE:
			colliderData->collider = CI_Allocate((Real2){0,0},0);// Allocate zeroed circle
			break;
		case COLL_SEGMENT:
			colliderData->collider = SG_Allocate((Real2){0,0},(Real2){0,0});// Allocate zeroed segment
			break;
		default:
			// Scream and die
			fprintf(stderr, "Collider type not recognized: %d\n", type);
			exit(1);
			break;
	}
	collider = CM_CreateComponent(COMP_COLLIDER, colliderData, &CD_Destructor, &CD_Mount, &CD_Update, dependenciesList);
	return collider;
}

void CD_Mount(Component *collider) 
{
	if (gColliderList == NULL) gColliderList = List_Nil();
	List_Append(gColliderList, collider);
}

void CD_Update(Component *collider) {}

void CD_DestroyData(void *colliderData)
{
	int type;

	type = ((ColliderData *)colliderData)->type;
	switch (type) {
		case COLL_POLYGON:
			PG_Destructor(((ColliderData *)colliderData)->collider);
			break;
		case COLL_CIRCLE:
			CI_Destroy(((ColliderData *)colliderData)->collider);
			break;
		case COLL_SEGMENT:
			SG_Destroy(((ColliderData *)colliderData)->collider);
			break;
		default:
			// Scream and die
			fprintf(stderr, "Collider type not recognized: %d\n", type);
			exit(1);
			break;

	}
}

void CD_Destructor(void *colliderData)
{
	int i;
	List *clist;

	CD_DestroyData(colliderData);

	clist = gColliderList;
	i = 0;
	LOOP_OVER(clist) {
		if (((Component *)List_Head(clist))->componentData == colliderData) {
			List_Delete(gColliderList, i);
			break;
		} 
		i++;
		
	}
	free(colliderData);
}

// Getters
int CD_GetType(Component *collider)
{
	return ((ColliderData *)collider->componentData)->type;
}
void *CD_GetCollider(Component *collider)
{
	return ((ColliderData *)collider->componentData)->collider;
}

// Setters
void CD_ResetCollider(Component *collider, int type)
{
	ColliderData *colliderData;
	CD_DestroyData(collider->componentData);
	colliderData = malloc(sizeof(ColliderData));
	colliderData->type = type;
	switch (type) {
		case COLL_POLYGON:
			colliderData->collider = PG_CreateEmpty();
			break;
		case COLL_CIRCLE:
			colliderData->collider = CI_Allocate((Real2){0,0},0);// Allocate zeroed circle
			break;
		case COLL_SEGMENT:
			colliderData->collider = SG_Allocate((Real2){0,0},(Real2){0,0});// Allocate zeroed segment
			break;
		default:
			// Scream and die
			fprintf(stderr, "Collider type not recognized: %d\n", type);
			exit(1);
			break;
	}
	collider->componentData = colliderData;
}

void CD_ComputeCollisions()
{
	int i, nColliders;
	List *clist;
	Component *currentCollider;
	
	nColliders = List_GetLength(gColliderList);

	for (i = 0; i < nColliders; i++) {
		clist = List_SliceFrom(gColliderList, i);
		currentCollider = List_Head(clist);
		clist = List_Tail(clist);
		LOOP_OVER(clist) {
			CD_CollidePair(currentCollider, List_Head(clist));
		}
	}
}

void CD_CollidePair(Component *collider1, Component *collider2)
{
	ColliderData *collider1Data, *collider2Data;

	collider1Data = collider1->componentData;
	collider2Data = collider2->componentData;

	// I apologize in advance for what I am about to do
	// Seriously
	switch (collider1Data->type) {
		case COLL_CIRCLE:
			switch (collider2Data->type) {
				case COLL_CIRCLE:
					CD_CollideCircleCircle(collider1, collider2);
					break;
				case COLL_SEGMENT:
					CD_CollideCircleSegment(collider1, collider2);
					break;
				case COLL_POLYGON:
					CD_CollideCirclePolygon(collider1, collider2);
					break;
			}
			break;
		case COLL_SEGMENT:
			switch (collider2Data->type) {
				case COLL_CIRCLE:
					CD_CollideCircleSegment(collider2, collider1);
					break;
				case COLL_SEGMENT:
					CD_CollideSegmentSegment(collider1, collider2);
					break;
				case COLL_POLYGON:
					CD_CollideSegmentPolygon(collider1, collider2);
					break;
			}
			break;
		case COLL_POLYGON:
			switch (collider2Data->type) {
				case COLL_CIRCLE:
					CD_CollideCirclePolygon(collider2, collider1);
					break;
				case COLL_SEGMENT:
					CD_CollideSegmentPolygon(collider2, collider1);
					break;
				case COLL_POLYGON:
					CD_CollidePolygonPolygon(collider1, collider2);
					break;
			}
			break;
	}
}

// Actual collision detection
// God save us
void CD_CollideCircleCircle(Component *circle1, Component *circle2)
{
	Circle *circle1Data, *circle2Data;
	double distance;

	circle1Data = CD_GetCollider(circle1);
	circle2Data = CD_GetCollider(circle2);
	distance = R2_Dist(
		TR_PosToRootSpace(GO_GetComponentFromOwner(circle1, COMP_TRANSFORM), circle1Data->center),
		TR_PosToRootSpace(GO_GetComponentFromOwner(circle2, COMP_TRANSFORM), circle2Data->center));

	if (distance > (circle1Data->radius + circle2Data->radius)) {
		// COLLISION!!
		printf("DEBUG: Collision happened circlecircle\n");
	}
}

void CD_CollideCircleSegment(Component *circle, Component *segment)
{
	Circle circleData;
	Segment segmentData; 
	Real2 v, w, lineIntersect;
	double distance;
	Component *circleTransform, *segmentTransform;

	circleTransform = GO_GetComponentFromOwner(circle, COMP_TRANSFORM);
	segmentTransform = GO_GetComponentFromOwner(segment, COMP_TRANSFORM);

	circleData = *(Circle *)CD_GetCollider(circle);
	segmentData = *(Segment *)CD_GetCollider(segment);

	circleData.center = TR_PosToRootSpace(circleTransform, circleData.center);
	segmentData.start = TR_PosToRootSpace(segmentTransform, segmentData.start);
	segmentData.end = TR_PosToRootSpace(segmentTransform, segmentData.end);

	v = R2_Sub(circleData.center, segmentData.start);
	w = R2_Sub(segmentData.end, segmentData.start);

	// (v ^ w) / |w|
	distance = fabs(R2_Wedge(v,w)/R2_Mag(w));

	// Rule out obvious non-collision
	if (distance > circleData.radius) {
		return;
	}

	// p0 + w * ( (w . v) / |w|^2)
	lineIntersect = R2_Add(segmentData.start, R2_ScalarMult(w, R2_Dot(w, v) / R2_MagSq(w)));
	printf("DEBUG: Potential circle-segment collision\n");
}

void CD_CollideCirclePolygon(Component *circle, Component *polygon) {}
void CD_CollideSegmentSegment(Component *segment0, Component *segment1)
{
	Component *transform0, *transform1;
	Segment segmentData0, segmentData1;

	transform0 = GO_GetComponentFromOwner(segment0, COMP_TRANSFORM);
	transform1 = GO_GetComponentFromOwner(segment1, COMP_TRANSFORM);
	segmentData0 = *(Segment *)CD_GetCollider(segment0);
	segmentData1 = *(Segment *)CD_GetCollider(segment1);

	segmentData0.start = TR_PosToRootSpace(transform0, segmentData0.start);
	segmentData0.end = TR_PosToRootSpace(transform0, segmentData0.end);
	segmentData1.start = TR_PosToRootSpace(transform1, segmentData1.start);
	segmentData1.end = TR_PosToRootSpace(transform1, segmentData1.end);


	if (SG_Intersect(segmentData0, segmentData1).didIntersect) {
		// COLLISION!!
		printf("DEBUG: Collision happened segmentsegment\n");
	}
}

void CD_CollideSegmentPolygon(Component *segment, Component *polygon) {}
void CD_CollidePolygonPolygon(Component *polygon1, Component *polygon2) {}
