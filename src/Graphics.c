#include "Graphics.h"


SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
double gYPixelsPerUnit = 0;
double gXPixelsPerUnit = 0;

// Local procedure declarations
void _GR_RenderShip(Object *ship, Object *camera, Transform relativeTransform);
void _GR_RenderTrail(Object *trail, Object *camera);
void _GR_RenderPolygonCollider(Object *obj, Object *camera);
void _GR_RenderProjectile(Object *obj, Object *camera, Transform relativeTransform);
void _GR_RenderRec(Object *root);
void _GR_DrawLine(Object *camera, Real2 start, Real2 end, int r, int g, int b, int a);
void _GR_DrawNetForce(Object *obj, Object *camera);
void _GR_SetUpRenderingGlobals(void);

void _GR_SetUpRenderingGlobals(void)
{
	Object *camera;

	camera = GO_GetCamera();
	gXPixelsPerUnit = SCREEN_WIDTH / GR_GetCameraWidth(camera);
	gYPixelsPerUnit = SCREEN_HEIGHT / GR_GetCameraHeight(camera);
}

void GR_Init(void)
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}
	
	// Create window
	gWindow = SDL_CreateWindow("shipsdl", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == NULL) {
		fprintf(stderr, "SDL could not create window! SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if(gRenderer == NULL) {
		fprintf(stderr, "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
		exit(1);
	}

	//Initialize renderer color
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

	
	// Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		fprintf(stderr, "SDL_image could not initialize! SDL_image error %s\n", IMG_GetError());
		exit(1);
	}

	// Load Asset data
	AS_LoadAll(gRenderer);
}

void GR_Render(Object *root)
{
	//Clear screen
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(gRenderer);
	_GR_SetUpRenderingGlobals();

	_GR_RenderRec(root);
	if (DRAW_FORCES) {
		List *forcesLog = PH_GetForcesLog();
		Force force;
		while (forcesLog != NULL) {
			force = *(Force *)List_Head(forcesLog);
			GR_DrawForce(force.obj, GO_GetCamera(), force.force, force.localPos);
			forcesLog = List_Tail(forcesLog);
		}
	}
	SDL_RenderPresent(gRenderer);
}

void _GR_RenderRec(Object *root)
{
	int xWithinRange, yWithinRange;
	Object *camera = GO_GetCamera();
	List *children;
	Transform relativeTransform;


	if (root == NULL) {
		fprintf(stderr, "Cannot render a null root\n");
		exit(1);
	}
	children = root->children;
	
	// Get global positions and check if object is in camera view
	relativeTransform  = GO_TransformToLocalSpace(camera, GO_TransformToRootSpaceObj(root));

	xWithinRange = relativeTransform.pos.x >= 0 && relativeTransform.pos.x <= GR_GetCameraWidth(camera);
	yWithinRange = relativeTransform.pos.y >= 0 && relativeTransform.pos.y <= GR_GetCameraHeight(camera);
	
	// Render if in view
	if (xWithinRange && yWithinRange) {
		switch (root->type) {
			case OBJ_SHIP:
				_GR_RenderShip(root, camera, relativeTransform);
				break;
			case OBJ_TRAIL:
				_GR_RenderTrail(root, camera);
				break;
			case OBJ_PROJECTILE:
				_GR_RenderProjectile(root, camera, relativeTransform);
				break;
		}
		if (root->collider.enabled && root->collider.type == COLL_POLYGON && DRAW_COLLIDERS) {
			_GR_RenderPolygonCollider(root, camera);
		}
		if (PH_IsPhysicsEnabled(root) && DRAW_FORCES) {
			_GR_DrawNetForce(root, camera);
		}
	}

	// Recursively render children
	//TODO: Recursion here is not efficient, this is just quick'n'dirty testing
	if (children != NULL) {
		while (children->next != NULL) {
			_GR_RenderRec(children->element);
			children = children->next;
		}
		_GR_RenderRec(children->element);
	}
}

void _GR_RenderTrail(Object *trail, Object *camera)
{
	int count, i, length;
	Trail *trailObj;
	Real2 localPointPos, prevPos;
	RGBA color;

	trailObj = (Trail *) trail->obj;
	color = trailObj->color;
	length = trailObj->length;
	i = trailObj->next;
	localPointPos = GO_PosToLocalSpace(camera, trailObj->points[i]);
	for (count = 0; count < length; i = (i + 1) % length, count++) {
		prevPos = localPointPos;
		localPointPos = GO_PosToLocalSpace(camera, trailObj->points[i]);
		SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, color.a);
		SDL_RenderDrawLine(
				gRenderer, (int) (prevPos.x * gXPixelsPerUnit),
				(int) (prevPos.y * gYPixelsPerUnit),
				(int) (localPointPos.x * gXPixelsPerUnit),
				(int) (localPointPos.y * gYPixelsPerUnit)
				);
	}
}

void _GR_RenderProjectile(Object *obj, Object *camera, Transform relativeTransform)
{
	double size;
	SDL_Point centerOfRotation;
	SDL_Rect currBlock;
	Projectile *proj;
	
	proj = (Projectile *)obj->obj;
	size = proj->size;
	centerOfRotation = (SDL_Point) { gXPixelsPerUnit * size/2, gYPixelsPerUnit * size/2 };
	currBlock = (SDL_Rect) {
		(int) (relativeTransform.pos.x * gXPixelsPerUnit),
		(int) (relativeTransform.pos.y * gYPixelsPerUnit),
		(int) ceil(gXPixelsPerUnit * proj->size),
		(int) ceil(gYPixelsPerUnit * proj->size),
	};

	LT_RenderRect(proj->projectileType->texture, &currBlock, NULL, -R2_AngleDeg(obj->physics.linearVel), &centerOfRotation, SDL_FLIP_NONE);
	_GR_RenderPolygonCollider(obj, camera);
}

void _GR_RenderPolygonCollider(Object *obj, Object *camera)
{
	List *currVertex;
	Real2 localPointPos, prevPos, firstPos;

	currVertex = ((Polygon *) obj->collider.collider)->vertices;
	firstPos = localPointPos = GO_PosToLocalSpace(camera, GO_PosToRootSpace(obj, *((Real2 *)List_Head(currVertex))));
	while (currVertex != NULL) {
		prevPos = localPointPos;
		localPointPos = GO_PosToLocalSpace(camera, GO_PosToRootSpace(obj, *((Real2 *)List_Head(currVertex))));
		currVertex = List_Tail(currVertex);
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
		SDL_RenderDrawRect(gRenderer, &(SDL_Rect) {
				(int) (localPointPos.x * gXPixelsPerUnit) - 2,
				(int) (localPointPos.y * gYPixelsPerUnit) - 2,
				4,
				4,
				});
		SDL_RenderDrawLine(
				gRenderer, (int) (prevPos.x * gXPixelsPerUnit),
				(int) (prevPos.y * gYPixelsPerUnit),
				(int) (localPointPos.x * gXPixelsPerUnit),
				(int) (localPointPos.y * gYPixelsPerUnit)
				);
	}
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
	SDL_RenderDrawLine(
			gRenderer, (int) (localPointPos.x * gXPixelsPerUnit),
			(int) (localPointPos.y * gYPixelsPerUnit),
			(int) (firstPos.x * gXPixelsPerUnit),
			(int) (firstPos.y * gYPixelsPerUnit)
			);
}

void _GR_DrawLine(Object *camera, Real2 start, Real2 end, int r, int g, int b, int a)
{
	start = GO_PosToLocalSpace(camera, start);
	end = GO_PosToLocalSpace(camera, end);

	SDL_SetRenderDrawColor(gRenderer, r, g, b, a);
	SDL_RenderDrawLine(
			gRenderer,
			start.x * gXPixelsPerUnit,
			start.y * gYPixelsPerUnit,
			end.x * gXPixelsPerUnit,
			end.y * gYPixelsPerUnit
			);
}

void GR_DrawForce(Object *obj, Object *camera, Real2 force, Real2 localPos)
{
	_GR_DrawLine(
		camera,
		GO_PosToRootSpace(obj, localPos),
		R2_Add(GO_PosToRootSpace(obj, localPos), R2_ScalarMult(force, 1.0/50)),
		0, 0, 255, 255
		);
}

void _GR_DrawNetForce(Object *obj, Object *camera)
{
	GR_DrawForce(obj, camera, PH_GetForceAccum(obj), PH_GetCenterOfMass(obj));
}

void _GR_RenderShip(Object *ship, Object *camera, Transform relativeTransform)
{
	//TODO: Figure out how to optimize by minimizing floating point operations
	//TODO: Block rendering seems somewhat unstable. Try using rotated basis instead
	int x, y, shipWidth, shipHeight;
	Block *block;
	List *holes;
	ShipHole *hole;
	SDL_Rect currBlock;
	SDL_Point centerOfRotation;
	Real2 rotatedBlockPos;
	Real2 rotatedBasisX, rotatedBasisY;
	
	holes = ((Ship *) ship->obj)->holes;
	centerOfRotation = (SDL_Point) { 0, 0 };
	shipWidth = ((Ship *) ship->obj)->width;
	shipHeight = ((Ship *) ship->obj)->height;
	rotatedBasisX = R2_RotateDeg((Real2) {gXPixelsPerUnit, 0}, relativeTransform.rot);
	rotatedBasisY = R2_RotateDeg((Real2) {0, gYPixelsPerUnit}, relativeTransform.rot);

	for (y = 0; y < shipHeight; y++) {
		for (x = 0; x < shipWidth; x++) {
			block = GO_ShipGetBlock(ship, x, y);
			rotatedBlockPos = R2_RotateDeg((Real2) { x, y }, relativeTransform.rot);
			currBlock = (SDL_Rect) {
				(int) ((relativeTransform.pos.x + rotatedBlockPos.x) * gXPixelsPerUnit),
				(int) ((relativeTransform.pos.y + rotatedBlockPos.y) * gYPixelsPerUnit),
				(int) ceil(gXPixelsPerUnit),
				(int) ceil(gYPixelsPerUnit),
			};
			switch (block->type) {
				case BLOCK_TEST:
					LT_RenderRect(block->material->texture, &currBlock, NULL, relativeTransform.rot, &centerOfRotation, SDL_FLIP_NONE);
					// Draw walls
					// Left
					LT_RenderStretch(
							block->walls[0]->texture, currBlock.x, currBlock.y, currBlock.w,
							currBlock.h, NULL, relativeTransform.rot, &centerOfRotation,
							SDL_FLIP_NONE);
					//Top
					LT_RenderStretch(
							block->walls[1]->texture, currBlock.x + rotatedBasisX.x, currBlock.y + rotatedBasisX.y, currBlock.w,
							currBlock.h, NULL, relativeTransform.rot + 90, &centerOfRotation,
							SDL_FLIP_NONE);
					//Right
					LT_RenderStretch(
							block->walls[2]->texture, currBlock.x + rotatedBasisX.x + rotatedBasisY.x, currBlock.y + rotatedBasisX.y + rotatedBasisY.y, currBlock.w,
							currBlock.h, NULL, relativeTransform.rot + 180, &centerOfRotation,
							SDL_FLIP_NONE);
					//Bottom
					LT_RenderStretch(
							block->walls[3]->texture, currBlock.x + rotatedBasisY.x, currBlock.y + rotatedBasisY.y, currBlock.w,
							currBlock.h, NULL, relativeTransform.rot + 270, &centerOfRotation,
							SDL_FLIP_NONE);
					break;
				case BLOCK_EMPTY:
					break;
				default:
					fprintf(stderr, "Unknown block type: %d\n", block->type);
					exit(1);
					break;
			}
		}
	}

	// Render holes (debug for now)
	while (holes != NULL) {
		hole = List_Head(holes);
		Real2 holepos = hole->pos;
		currBlock = (SDL_Rect) {
			(int) ((relativeTransform.pos.x + R2_RotateDeg(holepos, relativeTransform.rot).x) * gXPixelsPerUnit) -4,
			(int) ((relativeTransform.pos.y + R2_RotateDeg(holepos, relativeTransform.rot).y) * gYPixelsPerUnit) -4,
			8,
			8,
		};
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
		SDL_RenderFillRect(gRenderer, &currBlock);
		holes = List_Tail(holes);
	}

	//TODO: DEBUG UGLY STUFF. DELETE OR FIX vvvvvvvvvv
	rotatedBlockPos = R2_RotateDeg(ship->physics.centerOfMass, relativeTransform.rot);
	currBlock = (SDL_Rect) {
				(int) ((relativeTransform.pos.x + rotatedBlockPos.x) * gXPixelsPerUnit) -2,
				(int) ((relativeTransform.pos.y + rotatedBlockPos.y) * gYPixelsPerUnit) -2,
				4,
				4,
	};

	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
	SDL_RenderDrawRect(gRenderer, &currBlock);
	SDL_RenderDrawLine(gRenderer, currBlock.x, currBlock.y, (int) (relativeTransform.pos.x * gXPixelsPerUnit),
		(int) (relativeTransform.pos.y * gYPixelsPerUnit));

	//END_DEBUG^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
}

double GR_GetCameraWidth(Object *camera)
{
	return ((Camera *) camera->obj)->width;
}

double GR_GetCameraHeight(Object *camera)
{
	return GR_GetCameraWidth(camera) * (double) SCREEN_HEIGHT / (double) SCREEN_WIDTH;
}

SDL_Renderer *GR_GetMainRenderer(void)
{
	return gRenderer;
}
