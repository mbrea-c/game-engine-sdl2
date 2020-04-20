#include "Collider.h"
#include "Component.h"
#include "Ship.h"



typedef struct Intersection Intersection;

struct Intersection {
	Real2 pos;
	Real2 normal;
};

int colliderDependencies[] = { COMP_TRANSFORM };

Intersection _CD_FindProjectileShipIntersection(Object *ship, Object *bullet);

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
		default:
			// Scream and die
			fprintf(stderr, "Collider type not recognized: %d\n", type);
			exit(1);
			break;
	}
	collider = CM_CreateComponent(COMP_COLLIDER, colliderData, &CD_Destructor, &CD_Mount, dependenciesList);
	return collider;
}

void CD_Mount(Component *collider) {}

void CD_Destructor(void *colliderData)
{
	int type = ((ColliderData *)colliderData)->type;
	switch (type) {
		case COLL_POLYGON:
			PG_Destructor(((ColliderData *)colliderData)->collider);
			break;
		default:
			// Scream and die
			fprintf(stderr, "Collider type not recognized: %d\n", type);
			exit(1);
			break;

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
	CD_Destructor(collider->componentData);
	colliderData = malloc(sizeof(ColliderData));
	colliderData->type = type;
	switch (type) {
		case COLL_POLYGON:
			colliderData->collider = PG_CreateEmpty();
			break;
		default:
			// Scream and die
			fprintf(stderr, "Collider type not recognized: %d\n", type);
			exit(1);
			break;
	}
	collider->componentData = colliderData;
}



/*void CD_ShipHandleCollision(Object *ship, Object *other)*/
/*{*/
	/*switch (other->type) {*/
		/*case OBJ_PROJECTILE:*/
			/*break;*/
			
	/*}*/
/*}*/

//TODO: This procedure contains a silly amount of placeholder hacks
/*void CD_CheckCollision(Object *obj0, Object *obj1)*/
/*{*/
	/*int type0, type1;*/

	/*type0 = obj0->type;*/
	/*type1 = obj1->type;*/
	
	/*// No collision in current frame*/
	/*//TODO: Continuous collision should replace this shitty test*/
	/*if (!CD_MayCollide(obj0, obj1)) {*/
		/*return;*/
	/*}*/

	/*if ( (type0 == OBJ_PROJECTILE && type1 == OBJ_SHIP)*/
	  /*|| (type0 == OBJ_SHIP && type1 == OBJ_PROJECTILE)) {*/
		/*//TODO: CCD HERE*/
	/*}*/
/*}*/


/*Interval CD_ProjectOnAxis(Object *obj, Real2 axis)*/
/*{*/
	/*Interval interval;*/
	/*List *vertex; */
	/*Real2 axisNorm, rootVertex; */
	/*double projection;*/

	/*axisNorm = R2_Norm(axis);*/
	/*vertex = ((Polygon *) obj->collider.collider)->vertices;*/
	/*if (vertex == NULL) {*/
		/*interval.start = 0;*/
		/*interval.end = 0;*/
	/*} else {*/
		/*// Initialize interval to first element*/
		/*rootVertex = GO_PosToRootSpace(obj, *((Real2 *)List_Head(vertex)));*/
		/*projection = R2_ProjectOnUnit(rootVertex, axisNorm);*/
		/*interval.start = projection;*/
		/*interval.end = projection;*/
		/*vertex = List_Tail(vertex);*/
		/*// Run through rest of vertices*/
		/*while (vertex != NULL) {*/
			/*rootVertex = GO_PosToRootSpace(obj, *((Real2 *)List_Head(vertex)));*/
			/*projection = R2_ProjectOnUnit(rootVertex, axisNorm);*/
			/*interval.start = projection < interval.start ? projection : interval.start;*/
			/*interval.end = projection > interval.end ? projection : interval.end;*/
			/*vertex = List_Tail(vertex);*/
		/*}*/
	/*}*/

	/*return interval;*/
/*}*/

/*int CD_MayCollide(Object *obj1, Object *obj2)*/
/*{*/
	/*int mayCollide;*/
	/*Interval intervalObj1, intervalObj2;*/
	/*List *vertexList; */
	/*Real2 currVertex, nextVertex, axis; */

	/*mayCollide = 1;*/
	/*vertexList = ((Polygon *) obj1->collider.collider)->vertices;*/
	/*if (vertexList == NULL) {*/
		/*fprintf(stderr, "CD_MayCollide: obj1's collider is NULL\n");*/
		/*exit(1);*/
	/*}*/
	/*nextVertex = GO_PosToRootSpace(obj1, *((Real2 *)List_Head(vertexList)));*/
	/*while (List_HasTail(vertexList)) {*/
		/*currVertex = nextVertex;*/
		/*vertexList = List_Tail(vertexList);*/
		/*nextVertex = GO_PosToRootSpace(obj1, *((Real2 *)List_Head(vertexList)));*/
		/*axis = R2_Norm(R2_Normal(R2_Sub(nextVertex, currVertex)));*/
		/*intervalObj1 = CD_ProjectOnAxis(obj1, axis);*/
		/*intervalObj2 = CD_ProjectOnAxis(obj2, axis);*/
		/*mayCollide = mayCollide && IN_IsIntersecting(&intervalObj1, &intervalObj2);*/
	/*}*/
	/*vertexList = ((Polygon *) obj2->collider.collider)->vertices;*/
	/*if (vertexList == NULL) {*/
		/*fprintf(stderr, "CD_MayCollide: obj1's collider is NULL\n");*/
		/*exit(1);*/
	/*}*/
	/*nextVertex = GO_PosToRootSpace(obj2, *((Real2 *)List_Head(vertexList)));*/
	/*while (List_HasTail(vertexList)) {*/
		/*currVertex = nextVertex;*/
		/*vertexList = List_Tail(vertexList);*/
		/*nextVertex = GO_PosToRootSpace(obj2, *((Real2 *)List_Head(vertexList)));*/
		/*axis = R2_Norm(R2_Normal(R2_Sub(nextVertex, currVertex)));*/
		/*intervalObj1 = CD_ProjectOnAxis(obj1, axis);*/
		/*intervalObj2 = CD_ProjectOnAxis(obj2, axis);*/
		/*mayCollide = mayCollide && IN_IsIntersecting(&intervalObj1, &intervalObj2);*/
	/*}*/
	/*return mayCollide;*/
/*}*/

/*Real2 CD_PolygonGetFirstVertex(Object *obj)*/
/*{*/
	/*if (obj->collider.type != COLL_POLYGON) {*/
		/*fprintf(stderr, "Error: %s does not have a polygon collider\n", obj->name);*/
		/*exit(1);*/
	/*}*/
	/*return PG_GetFirstVertex(obj->collider.collider);*/
/*}*/

// ---------------------
// Local procedures

/*Intersection _CD_FindProjectileShipIntersection(Object *ship, Object *bullet)*/
/*{*/
	
/*}*/
