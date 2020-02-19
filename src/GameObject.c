#include "GameObject.h"

Object *gMainCamera = NULL;

// Declare local procedures
void _GO_LogObjectTreeRec(Object *root, FILE *outfile, int depth);
void _GO_ShipInitBlock(Object *ship, int x, int y, Block block);

void GO_CheckObjNotNull(Object *object)
{
	if (object == NULL) {
		fprintf(stderr, "Was given NULL object, did not expect that. Dying now.\n");
		exit(1);
	}
}

// General object procedures

Object *GO_CreateObject(int type, char *name, double x, double y, double rot,
		void *obj, Object *parent)
{
	Object *newObject = malloc(sizeof(Object));
	newObject->type = type;
	newObject->name = name;
	newObject->transform.pos.x = x;
	newObject->transform.pos.y = y;
	newObject->transform.rot = rot;
	newObject->obj = obj;
	newObject->parent = parent;
	newObject->children = NULL;
	newObject->physics.enabled = 0;
	newObject->collider.enabled = 0;
	if (parent != NULL) {
		GO_AddChild(parent, newObject);
	}
	return newObject;
}

void GO_AddChild(Object *parent, Object *child)
{
	GO_CheckObjNotNull(parent);
	GO_CheckObjNotNull(child);
	if (parent->children == NULL) {
		parent->children = List_Create(child);
	} else {
		List_Append(parent->children, child);
	}
}

Object *GO_GetParent(Object *object)
{
	GO_CheckObjNotNull(object);
	return object->parent;
}

Object *GO_GetChild(Object *object, char *name)
{
	List *children;

	GO_CheckObjNotNull(object);
	children = object->children;
	// No children
	if (children == NULL) {
		return NULL;
	}
	// Search children by name
	while (children->next != NULL) {
		if (strcmp(((Object *) children->element)->name, name))
			return (Object *) children->element;
		children = children->next;
	}
	// No entry found by name
	return NULL;
}

Transform GO_GetRootTransform(Object *object)
{
	//TODO: Fix rotation (limit to range [0, 360) )
	Object *current;
	Transform finalTransform;

	GO_CheckObjNotNull(object);
	current = object;
	finalTransform = current->transform;
	while (current->parent != NULL) {
		current = current->parent;
		finalTransform.pos = R2_Add(R2_RotateDeg(finalTransform.pos, current->transform.rot), current->transform.pos);
		finalTransform.rot = finalTransform.rot + current->transform.rot;
	}
	return finalTransform;
}

Transform GO_GetRelativeTransform_T(Transform *reference, Transform *target)
{
	//TODO: Fix rotation (limit to range [0, 360) )
	Transform relativeTransform;
	relativeTransform.pos.x = target->pos.x - reference->pos.x;
	relativeTransform.pos.y = target->pos.y - reference->pos.y;
	relativeTransform.pos = R2_RotateDeg(relativeTransform.pos, -reference->rot);
	relativeTransform.rot = target->rot - reference->rot;
	return relativeTransform;
}

Real2 GO_GetRootPositionFrom(Object *reference, Real2 localPos)
{
	Object *current;
	Real2 rootPos;

	GO_CheckObjNotNull(reference);
	current = reference;
	rootPos = localPos;
	while (current->parent != NULL) {
		rootPos = R2_Add(R2_RotateDeg(rootPos, current->transform.rot), current->transform.pos);
		current = current->parent;
	}
	return rootPos;
}

Real2 GO_GetLocalPositionTo(Object *reference, Real2 globalPos)
{
	Real2 localPos;

	localPos = R2_Sub(globalPos, reference->transform.pos);
	localPos = R2_RotateDeg(localPos, -reference->transform.rot);
	
	return localPos;
}

Real2 GO_PosToParentSpace(Object *obj, Real2 localPos)
{
	return R2_Add(obj->transform.pos, R2_RotateDeg(localPos, obj->transform.rot));
}

Real2 GO_PosToRootSpace(Object *obj, Real2 localPos)
{
	while (obj->parent != NULL) {
		localPos = GO_PosToParentSpace(obj, localPos);
		obj = obj->parent;
	}

	return localPos;
}

Real2 GO_PosToLocalSpace(Object *obj, Real2 rootPos)
{
	rootPos = R2_Sub(rootPos, GO_PosToRootSpaceObj(obj));
	rootPos = R2_RotateDeg(rootPos, -GO_RotToRootSpaceObj(obj));
	return rootPos;
}

Real2 GO_PosToRootSpaceObj(Object *obj)
{
	return GO_PosToRootSpace(obj, (Real2) {0, 0});
}

double GO_RotToParentSpace(Object *obj, double localRot)
{
	return localRot + obj->transform.rot;
}

double GO_RotToRootSpace(Object *obj, double localRot)
{
	while (obj->parent != NULL) {
		localRot = GO_RotToParentSpace(obj, localRot);
		obj = obj->parent;
	}

	return localRot;
}

double GO_RotToLocalSpace(Object *obj, double rootRot)
{
	return rootRot - GO_RotToRootSpace(obj, obj->transform.rot);
}

double GO_RotToRootSpaceObj(Object *obj)
{
	return GO_RotToRootSpace(obj, 0);
}

Transform GO_TransformToParentSpace(Object *obj, Transform localTransform)
{
	localTransform.pos = GO_PosToParentSpace(obj, localTransform.pos);
	localTransform.rot = GO_RotToParentSpace(obj, localTransform.rot);
	return localTransform;
}

Transform GO_TransformToRootSpace(Object *obj, Transform localTransform)
{
	localTransform.pos = GO_PosToRootSpace(obj, localTransform.pos);
	localTransform.rot = GO_RotToRootSpace(obj, localTransform.rot);
	return localTransform;
}

Transform GO_TransformToLocalSpace(Object *obj, Transform rootTransform)
{
	rootTransform.pos = GO_PosToLocalSpace(obj, rootTransform.pos);
	rootTransform.rot = GO_RotToLocalSpace(obj, rootTransform.rot);
	return rootTransform;
}

Transform GO_TransformToRootSpaceObj(Object *obj)
{
	return GO_TransformToRootSpace(obj, (Transform) {(Real2) {0, 0}, 0});
}

// Gets position information relative to parent of given object
Real2 GO_GetParentRelativePos(Object *object, Real2 pos)
{
	Real2 rotatedPos;

	GO_CheckObjNotNull(object);
	GO_CheckObjNotNull(object->parent);
	rotatedPos = R2_RotateDeg(pos, object->transform.rot);

	return R2_Add(object->transform.pos, rotatedPos);
}

void GO_LogObjectTree(Object *root, FILE *outfile)
{
	_GO_LogObjectTreeRec(root, outfile, 0);
}

void _GO_LogObjectTreeRec(Object *root, FILE *outfile, int depth)
{
	int i;
	List *children;

	GO_CheckObjNotNull(root);
	children = root->children;

	// Indent according to depth in tree
	for (i = 0; i < depth; i++) {
		fprintf(outfile, "  ");
	}
	fprintf(outfile, "|-%s\n", root->name);

	// Recursively call on all children
	if (children != NULL) {
		while (children->next != NULL) {
			_GO_LogObjectTreeRec(children->element, outfile, depth + 1);
			children = children->next;
		}
		_GO_LogObjectTreeRec(children->element, outfile, depth + 1);
	}
}

// Ship procedures

Object *GO_CreateEmptyShip(char *name, double x, double y, double rot, Object *parent, int width, int height)
{
	printf("DEBUG: Creating ship %s\n", name);
	int i, j;
	Ship *shipObj = malloc(sizeof(Ship));
	shipObj->width = width;
	shipObj->height = height;
	shipObj->blocks = malloc(width * height * sizeof(Block));
	shipObj->holes = NULL;
	Object *newShip = GO_CreateObject(OBJ_SHIP, name, x, y, rot, shipObj, parent);

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			_GO_ShipInitBlock(newShip, i, j, GO_CreateUnwalledBlock(BLOCK_EMPTY));
		}
	}

	// Initialize physics information
	newShip->physics.enabled = 1;
	newShip->physics.angularVel = 0;
	newShip->physics.linearVel = (Real2) { 0, 0 };
	PH_UpdateShipPhysicsData(newShip);

	// Initialize collider
	newShip->collider.enabled = 1;
	newShip->collider.type = COLL_POLYGON;
	CD_GenerateShipCollider(newShip);

	printf("DEBUG: Created ship %s\n", name);
	return newShip;
}

Block *GO_ShipGetBlock(Object *ship, int x, int y)
{
	GO_CheckObjNotNull(ship);
	int width, height;
	width = ((Ship *) ship->obj)->width;
	height = ((Ship *) ship->obj)->height;
	Block *blocks = ((Ship *) ship->obj)->blocks; 
	if (x < 0 || x >= width) {
		fprintf(stderr, "GO_ShipGetBlock: Array x coordinate out of bounds!\n");
		exit(1);
	} else if (y < 0 || y >= height) {
		fprintf(stderr, "GO_ShipGetBlock: Array y coordinate out of bounds!\n");
		exit(1);
	}
	return (blocks + y * width + x);
}

void _GO_ShipInitBlock(Object *ship, int x, int y, Block block)
{
	*(GO_ShipGetBlock(ship, x, y)) = block;
}

void GO_ShipSetBlock(Object *ship, int x, int y, Block block)
{
	_GO_ShipInitBlock(ship, x, y, block);
	PH_UpdateShipPhysicsData(ship);
	CD_GenerateShipCollider(ship);
}

int GO_ShipGetHeight(Object *ship)
{
	return ((Ship *) ship->obj)->height;
}

int GO_ShipGetWidth(Object *ship)
{
	return ((Ship *) ship->obj)->width;
}

void GO_ShipSetBlockWall(Object *ship, int x, int y, int wall, int wallType)
{
	GO_ShipGetBlock(ship, x, y)->walls[wall] = gWallTypes[wallType];
}

void GO_ShipCloseWithWalls(Object *ship, int wallType)
{
	int width, height, x, y;
	Block *block;

	width = GO_ShipGetWidth(ship);
	height = GO_ShipGetHeight(ship);

	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {
			block = GO_ShipGetBlock(ship, x, y);	
			if (block->type != BLOCK_EMPTY) {
				if (x <= 0 || GO_ShipGetBlock(ship, x-1, y)->type == BLOCK_EMPTY) {
					block->walls[LEFT_WALL] = gWallTypes[wallType];
				}
				if (y <= 0 || GO_ShipGetBlock(ship, x, y-1)->type == BLOCK_EMPTY) {
					block->walls[TOP_WALL] = gWallTypes[wallType];
				}
				if (x >= width - 1 || GO_ShipGetBlock(ship, x+1, y)->type == BLOCK_EMPTY) {
					block->walls[RIGHT_WALL] = gWallTypes[wallType];
				}
				if (y >= height - 1 || GO_ShipGetBlock(ship, x, y+1)->type == BLOCK_EMPTY) {
					block->walls[BOTTOM_WALL] = gWallTypes[wallType];
				}
			}
		}
	}
}

void GO_ShipAddHole(Object *ship, Real2 damagePos)
{
	ShipHole *hole;
	Ship *shipObj;
	int hx, hy;

	shipObj = (Ship *) ship->obj;
	hx = (int) damagePos.x;
	hy = (int) damagePos.y;

	hole = malloc(sizeof(ShipHole));
	hole->pos = damagePos;
	// Find two closest block to damage position
	// block it lies on
	hole->xBlock1 = hx;
	hole->yBlock1 = hy;
	// second closest block
	hole->xBlock2 = damagePos.x - hx < hx + 1 - damagePos.x ? hx - 1 : hx + 1;
	hole->yBlock2 = damagePos.y - hy < hy + 1 - damagePos.y ? hy - 1 : hy + 1;

	// Add hole info to ship hole list
	if (shipObj->holes == NULL) {
		shipObj->holes = List_Create(hole);
	} else {
		List_Append(shipObj->holes, hole);
	}
}

// Projectile procedures

Object *GO_CreateProjectile(char *name, double x, double y, Object *parent, int projectileType, double size)
{
	Object *projObj;
	Projectile *proj = malloc(sizeof(Projectile));

	proj->projectileType = gProjectileTypes[projectileType];
	proj->size = size;
	projObj = GO_CreateObject(OBJ_PROJECTILE, name, x, y, 0, proj, parent);

	// Initialize physics
	PH_SetPhysicsEnabled(projObj, 1);
	PH_SetLinearVelocity(projObj, (Real2) {0, 0});
	PH_SetAngularVelocity(projObj, 0);
	PH_SetCenterOfMass(projObj, (Real2) {size/2, size/2});
	PH_SetMass(projObj, gProjectileTypes[projectileType]->mass);

	// Initialize collider
	projObj->collider.enabled = 1;
	projObj->collider.type = COLL_POLYGON;
	CD_GenerateProjectileCollider(projObj);

	return projObj;
}

// Block procedures

Block GO_CreateUnwalledBlock(int type)
{
	int k;
	Block newBlock;
	newBlock.type = type;
	newBlock.material = gMaterials[type];
	for (k = 0; k < 4; k++) {
		newBlock.walls[k] = gWallTypes[WALL_EMPTY];
	}
	return newBlock;
}

// Camera procedures

Object *GO_CreateCamera(double x, double y, char *name, Object *parent, double width)
{
	Camera *cameraObj = malloc(sizeof(Camera));
	cameraObj->width = width;
	
	Object *newCamera = GO_CreateObject(OBJ_CAMERA, name, x, y, 0, cameraObj, parent);
	GO_SetCamera(newCamera);
	return newCamera;
}

void GO_SetCamera(Object *camera)
{
	if (gMainCamera != NULL) {
		fprintf(stderr, "2 cameras 5 me\n");
		exit(1);
	}
	gMainCamera = camera;
}

Object *GO_GetCamera(void)
{
	if (gMainCamera == NULL) {
		fprintf(stderr, "NO CAMERA REEEEEEEE. WILL COMMIT NOT ALIVE.\n");
		exit(1);
	}
	return gMainCamera;

}

// Trail procedures

Object *GO_CreateTrail(char *name, Object *parent, int length)
{
	Trail *trailObj = malloc(sizeof(Trail));
	trailObj->length = length;
	trailObj->points = calloc(length, sizeof(Real2));
	trailObj->next = 0;
	Object *newTrail = GO_CreateObject(OBJ_TRAIL, name, 0, 0, 0, trailObj, parent);
	return newTrail;
}

void GO_PushToTrail(Object *trail, Real2 globalPoint)
{
	Trail *trailObj = (Trail *) trail->obj;
	trailObj->points[trailObj->next] = globalPoint;
	trailObj->next = (trailObj->next + 1) % trailObj->length;
}
