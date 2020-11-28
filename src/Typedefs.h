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

enum ComponentTypes {
	COMP_SHIP, 
	COMP_CAMERA,
	COMP_TRAIL,
	COMP_PROJECTILE,
	COMP_TRANSFORM,
	COMP_PHYSICS,
	COMP_COLLIDER,
	COMP_TESTMOVER,
	COMP_TESTMOUSEFORCE,
};

enum ColliderTypes {
	COLL_POLYGON,
	COLL_CIRCLE,
	COLL_SEGMENT,
	COLL_COMPOUND,
};

// forward declarations 
// allow types to contain pointers to themselves
typedef struct Component Component;
// Componnent data types
typedef struct TransformData TransformData;
typedef struct ShipData ShipData;
typedef struct PhysicsData PhysicsData;
typedef struct ColliderData ColliderData;
typedef struct ProjectileData ProjectileData;
typedef struct TrailData TrailData;
typedef struct CameraData CameraData;
typedef struct TestMouseForceData TestMouseForceData;

typedef struct Object Object;
typedef struct Block Block;
typedef struct Material Material;
typedef struct WallType WallType;
typedef struct ProjectileType ProjectileType;
typedef struct ShipHole ShipHole;
typedef struct Force Force;
typedef struct RGBA RGBA;

struct RGBA {
	int r;
	int g;
	int b;
	int a;
};

struct Force {
	Component *physics;
	Real2 force;
	Real2 localPos;
};

struct TransformData {
	Real2 pos;
	double rot;
};

struct PhysicsData {
	// General
	double mass;
	double momentOfInertia;
	Real2 linearVel;
	double angularVel;
	Real2 centerOfMass;
	// Needed for physics updaters
	Real2 nextLinearVel;
	double nextAngularVel;
	double nextRot;
	Real2 nextCenterOfMass;
	// Accumulators
	Real2 forceAccum;
	double torqueAccum;
	// Old transform data for collisions
	Real2 prevPos;
	double prevRot;
};

struct ColliderData {
	int type;
	void *collider;
};

struct CameraData {
	double width;
};

struct TestMouseForceData {
	Real2 localDragPoint;
	double springConstant;
};

struct Object {
	char *name;
	int id;
	Object *parent;
	List *children;
	List *components;
};

struct Component {
	int type;
	Object *owner;
	void *componentData;
	void (*destructor)(void *);
	void (*mount)(Component *);
	void (*update)(Component *);
	List *dependencies;
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

struct ShipData {
	int width;
	int height;
	Block *blocks;
	List *holes;
};

struct ProjectileData {
	ProjectileType *projectileType;
	double size;
};

struct ProjectileType {
	int type;
	LTexture *texture;
	double mass;
};

struct TrailData {
	int length;
	int next;
	Real2 *points;
	RGBA color;
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
