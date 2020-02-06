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
typedef struct Trail Trail;
typedef struct Graphics Graphics;
typedef struct Block Block;
typedef struct Physics Physics;
typedef struct Collider Collider;
typedef struct Material Material;
typedef struct WallType WallType;

struct Physics {
	int enabled;
	Real2 linearVel;
	double angularVel;
	Real2 centerOfMass;
	double mass;
	double momentOfInertia;
};

struct Collider {
	int enabled;
	int type;
	void *collider;
};

struct Transform {
	Real2 pos;
	double rot;
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

struct Graphics {
	LTexture *texture;
};

struct Block {
	int type;
	Material *material;
	WallType *walls[4];
};

struct Ship {
	int width;
	int height;
	Block *blocks;
	Graphics graphics;
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
