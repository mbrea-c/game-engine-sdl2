#include "Projectile.h"
#include "Assets.h"
#include "GameObject.h"
#include "Collider.h"
#include "Component.h"

int projectileDependencies[] = { COMP_TRANSFORM, COMP_PHYSICS, COMP_COLLIDER };

// Local procedure declarations
void PR_UpdateCollider(Component *projectile);

// Basic

Component *PR_Create(int projectileType, double size)
{
	unsigned int i;
	Component *projectile;
	ProjectileData *projectileData;
	List *dependenciesList;

	projectileData = malloc(sizeof(ProjectileData));

	projectileData->projectileType = gProjectileTypes[projectileType];
	projectileData->size = size;

	dependenciesList = List_Nil();
	for (i = 0; i < sizeof(projectileDependencies)/sizeof(int); i++) {
		List_Append(dependenciesList, &projectileDependencies[i]);
	}
	projectile = CM_CreateComponent(COMP_PROJECTILE, projectileData, &PR_Destructor, &PR_Mount, dependenciesList);

	return projectile;
}

void PR_Mount(Component *projectile)
{
	double size, mass;
	Component *physics, *collider;

	size = PR_GetSize(projectile);
	mass = PR_GetMass(projectile);
	physics = GO_GetComponent(GO_GetComponentOwner(projectile), COMP_PHYSICS);
	collider = GO_GetComponent(GO_GetComponentOwner(projectile), COMP_COLLIDER);
	// Initialize physics
	PH_SetLinearVelocity(physics, (Real2) {0, 0});
	PH_SetAngularVelocity(physics, 0);
	PH_SetCenterOfMass(physics, (Real2) {size/2, size/2});
	PH_SetMass(physics, mass);
	PH_SetMomentOfInertia(physics, size * size * mass / 2);

	// Initialize collider
	PR_UpdateCollider(projectile);
}

void PR_Destructor(void *projectileData)
{
	free(projectileData);
}

// Getters
int PR_GetType(Component *projectile)
{
	return ((ProjectileData *)projectile->componentData)->projectileType->type;
}

double PR_GetMass(Component *projectile)
{
	return ((ProjectileData *)projectile->componentData)->projectileType->mass;
}

double PR_GetSize(Component *projectile)
{
	return ((ProjectileData *)projectile->componentData)->size;
}

LTexture *PR_GetTexture(Component *projectile)
{
	return ((ProjectileData *)projectile->componentData)->projectileType->texture;
}

// Local procedures
void PR_UpdateCollider(Component *projectile)
{
	double size;	
	Polygon *polygon;
	Component *collider;

	collider = GO_GetComponentFromOwner(projectile, COMP_COLLIDER);
	CD_ResetCollider(collider, COLL_POLYGON);
	polygon = CD_GetCollider(collider);
	size = PR_GetSize(projectile);
	PG_AppendVertex(polygon, (Real2) {size/2, size/2});
}
