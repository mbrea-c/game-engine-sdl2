#ifndef TYPEDEFS_H
#define TYPEDEFS_H

// This file contains types that will be used throughout the codebase.
// They are declared in a separate file to prevent include conflicts.
// Some utility types (e.g. Real2 or LTexture) remain in their own files,
// since they don't include other files from this project.

#include "Real2.h"
#include "List.h"
#include "LTexture.h"

// enums
enum BlockTypes {
	BLOCK_TEST,
	BLOCK_EMPTY,
	BLOCK_COUNT
};

enum WallTypes {
	WALL_LIGHT,
	WALL_EMPTY,
	WALL_COUNT
};

enum ProjectileTypes {
	PROJ_BULLET,
	PROJ_COUNT
};

enum WallPos {
	LEFT_WALL,
	TOP_WALL,
	RIGHT_WALL,
	BOTTOM_WALL
};

enum ObjectTypes {
	OBJ_WORLD_NODE,
	OBJ_SHIP, 
	OBJ_CAMERA,
	OBJ_TRAIL,
	OBJ_PROJECTILE
};

enum ColliderTypes {
	COLL_POLYGON,
};

// forward declarations 
// allow types to contain pointers to themselves
typedef struct Camera Camera;
typedef struct Transform Transform;
typedef struct Object Object;
typedef struct Ship Ship;
typedef struct Projectile Projectile;
typedef struct Trail Trail;
typedef struct Block Block;
typedef struct Physics Physics;
typedef struct Collider Collider;
typedef struct Material Material;
typedef struct WallType WallType;
typedef struct ProjectileType ProjectileType;
typedef struct ShipHole ShipHole;

struct Transform {
	Real2 pos;
	double rot;
};

struct Physics {
	// Constants
	int enabled;
	double mass;
	double momentOfInertia;
	// Accumulators
	Real2 forceAccum;
	double torqueAccum;
	// Mutable data
	Real2 linearVel;
	double angularVel;
	Real2 centerOfMass;
	// Helper for physics updaters
	Real2 nextLinearVel;
	double nextAngularVel;
	double nextRot;
	Real2 nextCenterOfMass;
	// Old transform for collisions
	Transform prevTransform;
};

struct Collider {
	int enabled;
	int type;
	void *collider;
};

struct Camera {
	double width;
};

//TODO: Initialize object collider to disabled by default. Shit gon break
struct Object {
	int type; // ObjectTypes
	char *name;
	Transform transform;
	Physics physics;
	Collider collider;
	void *obj;
	Object *parent;
	List *children;
};

struct Block {
	int type;
	Material *material;
	WallType *walls[4];
};

struct ShipHole {
	Real2 pos;
	int xBlock1, yBlock1, xBlock2, yBlock2;
};

struct Ship {
	int width;
	int height;
	Block *blocks;
	List *holes;
};

struct Projectile {
	ProjectileType *projectileType;
	double size;
};

struct ProjectileType {
	int type;
	LTexture *texture;
	double mass;
};

struct Trail {
	int length;
	int next;
	Real2 *points;
};

struct WallType {
	int type;
	LTexture *texture;
	double mass;
};

struct Material {
	int type;
	LTexture *texture;
	double mass;
};

#endif //TYPEDEFS_H
