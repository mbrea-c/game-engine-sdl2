#include "Graphics.h"
#include "Transform.h"
#include "Collider.h"
#include "Trail.h"
#include "Projectile.h"
#include "Ship.h"
#include "Camera.h"
#include "Input.h"


SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
Object *gCamera;
double gYPixelsPerUnit = 0;
double gXPixelsPerUnit = 0;

// Local procedure declarations
void _GR_RenderShip(Object *ship, Real2 relativePos, double relativeRot);
void _GR_RenderTrail(Object *trail);
void _GR_RenderPolygonCollider(Object *obj);
void _GR_RenderProjectile(Object *obj, Real2 relativePos, double relativeRot);
void _GR_RenderRec(Object *root);
void _GR_DrawLine(Real2 start, Real2 end, int r, int g, int b, int a);
void _GR_DrawNetForce(Object *obj);
void _GR_DrawCursor();
void _GR_SetUpRenderingGlobals(void);

void _GR_SetUpRenderingGlobals(void)
{
	Object *camera;

	camera = gCamera;
	gXPixelsPerUnit = SCREEN_WIDTH / GR_GetCameraWidth(camera);
	gYPixelsPerUnit = SCREEN_HEIGHT / GR_GetCameraHeight(camera);
}

void GR_Init(Object *camera)
{
	gCamera = camera;
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
		while (forcesLog != NULL && !List_IsEmpty(forcesLog)) {
			force = *(Force *)List_Head(forcesLog);
			GR_DrawForce(GO_GetComponentOwner(force.physics), force.force, force.localPos);
			forcesLog = List_Tail(forcesLog);
		}
	}
	_GR_DrawCursor();
	SDL_RenderPresent(gRenderer);
}

void _GR_RenderRec(Object *root)
{
	int xWithinRange, yWithinRange;
	Component *cameraTransform, *rootTransform, *rootCollider;
	List *children;
	Real2  relativePos;
	double relativeRot;


	if (root == NULL) {
		fprintf(stderr, "Cannot render a null root\n");
		exit(1);
	}
	children = root->children;
	cameraTransform = GO_GetComponent(gCamera, COMP_TRANSFORM);
	rootTransform   = GO_GetComponent(root,    COMP_TRANSFORM);
	rootCollider    = GO_GetComponent(root,    COMP_COLLIDER);
	
	// Get global positions and check if object is in camera view
	relativePos  = TR_PosToLocalSpace(cameraTransform, TR_PosToRootSpaceObj(rootTransform));
	relativeRot  = TR_RotToLocalSpace(cameraTransform, TR_RotToRootSpaceObj(rootTransform));

	xWithinRange = relativePos.x >= -GR_GetCameraWidth() && relativePos.x <= 2*GR_GetCameraWidth();
	yWithinRange = relativePos.y >= -GR_GetCameraHeight() && relativePos.y <= 2*GR_GetCameraHeight();
	
	// Render if in view
	if (xWithinRange && yWithinRange) {
		if (GO_HasComponent(root, COMP_SHIP)) {
			_GR_RenderShip(root, relativePos, relativeRot);
		} else if (GO_HasComponent(root, COMP_TRAIL)) {
			_GR_RenderTrail(root);
		} else if (GO_HasComponent(root, COMP_PROJECTILE)) {
			_GR_RenderProjectile(root, relativePos, relativeRot);
		}

		if (rootCollider != NULL && CD_GetType(rootCollider) == COLL_POLYGON && DRAW_COLLIDERS) {
			_GR_RenderPolygonCollider(root);
		}
		if (GO_HasComponent(root, COMP_PHYSICS) && DRAW_FORCES) {
			_GR_DrawNetForce(root);
		}
	}

	// Recursively render children
	//TODO: Recursion here is not efficient, this is just quick'n'dirty testing
	while (!List_IsEmpty(children)) {
		_GR_RenderRec(children->element);
		children = List_Tail(children);
	}
}

void _GR_RenderTrail(Object *trail)
{
	int count, i, length;
	Component *trailComponent, *cameraTransform;
	Real2 localPointPos, prevPos;
	RGBA color;

	trailComponent  = GO_GetComponent(trail, COMP_TRAIL);
	cameraTransform = GO_GetComponent(gCamera, COMP_TRANSFORM);
	color = TL_GetColor(trailComponent);
	length = TL_GetLength(trailComponent);
	i = TL_GetNext(trailComponent);
	localPointPos = TR_PosToLocalSpace(cameraTransform, TL_GetPoint(trailComponent, i));
	for (count = 0; count < length; i = (i + 1) % length, count++) {
		prevPos = localPointPos;
		localPointPos = TR_PosToLocalSpace(cameraTransform, TL_GetPoint(trailComponent, i));
		SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, color.a);
		SDL_RenderDrawLine(
				gRenderer, (int) (prevPos.x * gXPixelsPerUnit),
				(int) (prevPos.y * gYPixelsPerUnit),
				(int) (localPointPos.x * gXPixelsPerUnit),
				(int) (localPointPos.y * gYPixelsPerUnit)
				);
	}
}

void _GR_RenderProjectile(Object *obj, Real2 relativePos, double relativeRot)
{
	double size;
	SDL_Point centerOfRotation;
	SDL_Rect currBlock;
	Component *projectileComponent, *projectilePhysics;
	
	projectileComponent = GO_GetComponent(obj, COMP_PROJECTILE);
	projectilePhysics   = GO_GetComponent(obj, COMP_PHYSICS);
	size = PR_GetSize(projectileComponent);
	centerOfRotation = (SDL_Point) { gXPixelsPerUnit * size/2, gYPixelsPerUnit * size/2 };
	currBlock = (SDL_Rect) {
		(int) (relativePos.x * gXPixelsPerUnit),
		(int) (relativePos.y * gYPixelsPerUnit),
		(int) ceil(gXPixelsPerUnit * size),
		(int) ceil(gYPixelsPerUnit * size),
	};

	LT_RenderRect(PR_GetTexture(projectileComponent), &currBlock, NULL, -R2_AngleDeg(PH_GetLinearVelocity(projectilePhysics)), &centerOfRotation, SDL_FLIP_NONE);
}

void _GR_RenderPolygonCollider(Object *obj)
{
	List *currVertex;
	Real2 localPointPos, prevPos, firstPos;
	Component *colliderComponent, *cameraTransform, *objTransform;

	colliderComponent = GO_GetComponent(obj, COMP_COLLIDER);
	cameraTransform   = GO_GetComponent(gCamera, COMP_TRANSFORM);
	objTransform      = GO_GetComponent(obj, COMP_TRANSFORM);
	currVertex = ((Polygon *)CD_GetCollider(colliderComponent))->vertices;
	firstPos = localPointPos = TR_PosToLocalSpace(cameraTransform, TR_PosToRootSpace(objTransform, *((Real2 *)List_Head(currVertex))));
	while (!List_IsEmpty(currVertex)) {
		prevPos = localPointPos;
		localPointPos = TR_PosToLocalSpace(cameraTransform, TR_PosToRootSpace(objTransform, *((Real2 *)List_Head(currVertex))));
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

void _GR_DrawLine(Real2 start, Real2 end, int r, int g, int b, int a)
{
	Component *cameraTransform;

	cameraTransform = GO_GetComponent(gCamera, COMP_TRANSFORM);
	start = TR_PosToLocalSpace(cameraTransform, start);
	end = TR_PosToLocalSpace(cameraTransform, end);

	SDL_SetRenderDrawColor(gRenderer, r, g, b, a);
	SDL_RenderDrawLine(
			gRenderer,
			start.x * gXPixelsPerUnit,
			start.y * gYPixelsPerUnit,
			end.x * gXPixelsPerUnit,
			end.y * gYPixelsPerUnit
			);
}

void GR_DrawForce(Object *obj, Real2 force, Real2 localPos)
{
	Component *objTransform;

	objTransform = GO_GetComponent(obj, COMP_TRANSFORM);
	_GR_DrawLine(
		TR_PosToRootSpace(objTransform, localPos),
		R2_Add(TR_PosToRootSpace(objTransform, localPos), R2_ScalarMult(force, 1.0/50)),
		0, 0, 255, 255
		);
}

void _GR_DrawNetForce(Object *obj)
{
	Component *objPhysics;

	objPhysics = GO_GetComponent(obj, COMP_PHYSICS);
	GR_DrawForce(obj, PH_GetForceAccum(objPhysics), PH_GetCenterOfMass(objPhysics));
}

void _GR_RenderShip(Object *ship, Real2 relativePos, double relativeRot)
{
	// TODO: Rewrite this by storing ship texture and updating on block change
	int x, y, shipWidth, shipHeight;
	Component *shipComponent, *shipPhysics;
	Block *block;
	List *holes;
	ShipHole *hole;
	SDL_Rect currBlock;
	SDL_Point centerOfRotation;
	Real2 rotatedBlockPos;
	Real2 rotatedBasisX, rotatedBasisY;
	
	shipComponent = GO_GetComponent(ship, COMP_SHIP);
	shipPhysics   = GO_GetComponent(ship, COMP_PHYSICS);
	holes = SH_GetHoles(shipComponent);
	centerOfRotation = (SDL_Point) { 0, 0 };
	shipWidth = SH_GetWidth(shipComponent);
	shipHeight = SH_GetHeight(shipComponent);
	rotatedBasisX = R2_RotateDeg((Real2) {gXPixelsPerUnit, 0}, relativeRot);
	rotatedBasisY = R2_RotateDeg((Real2) {0, gYPixelsPerUnit}, relativeRot);

	for (y = 0; y < shipHeight; y++) {
		for (x = 0; x < shipWidth; x++) {
			block = SH_GetBlock(shipComponent, x, y);
			rotatedBlockPos = R2_RotateDeg((Real2) { x, y }, relativeRot);
			currBlock = (SDL_Rect) {
				(int) ((relativePos.x + rotatedBlockPos.x) * gXPixelsPerUnit),
				(int) ((relativePos.y + rotatedBlockPos.y) * gYPixelsPerUnit),
				(int) ceil(gXPixelsPerUnit),
				(int) ceil(gYPixelsPerUnit),
			};
			switch (block->type) {
				case BLOCK_TEST:
					LT_RenderRect(block->material->texture, &currBlock, NULL, relativeRot, &centerOfRotation, SDL_FLIP_NONE);
					// Draw walls
					// Left
					LT_RenderStretch(
							block->walls[0]->texture, currBlock.x, currBlock.y, currBlock.w,
							currBlock.h, NULL, relativeRot, &centerOfRotation,
							SDL_FLIP_NONE);
					//Top
					LT_RenderStretch(
							block->walls[1]->texture, currBlock.x + rotatedBasisX.x, currBlock.y + rotatedBasisX.y, currBlock.w,
							currBlock.h, NULL, relativeRot + 90, &centerOfRotation,
							SDL_FLIP_NONE);
					//Right
					LT_RenderStretch(
							block->walls[2]->texture, currBlock.x + rotatedBasisX.x + rotatedBasisY.x, currBlock.y + rotatedBasisX.y + rotatedBasisY.y, currBlock.w,
							currBlock.h, NULL, relativeRot + 180, &centerOfRotation,
							SDL_FLIP_NONE);
					//Bottom
					LT_RenderStretch(
							block->walls[3]->texture, currBlock.x + rotatedBasisY.x, currBlock.y + rotatedBasisY.y, currBlock.w,
							currBlock.h, NULL, relativeRot + 270, &centerOfRotation,
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
	while (!List_IsEmpty(holes)) {
		hole = List_Head(holes);
		Real2 holepos = hole->pos;
		currBlock = (SDL_Rect) {
			(int) ((relativePos.x + R2_RotateDeg(holepos, relativeRot).x) * gXPixelsPerUnit) -4,
			(int) ((relativePos.y + R2_RotateDeg(holepos, relativeRot).y) * gYPixelsPerUnit) -4,
			8,
			8,
		};
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
		SDL_RenderFillRect(gRenderer, &currBlock);
		holes = List_Tail(holes);
	}

	//TODO: DEBUG UGLY STUFF. DELETE OR FIX vvvvvvvvvv
	rotatedBlockPos = R2_RotateDeg(PH_GetCenterOfMass(shipPhysics), relativeRot);
	currBlock = (SDL_Rect) {
				(int) ((relativePos.x + rotatedBlockPos.x) * gXPixelsPerUnit) -2,
				(int) ((relativePos.y + rotatedBlockPos.y) * gYPixelsPerUnit) -2,
				4,
				4,
	};

	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
	SDL_RenderDrawRect(gRenderer, &currBlock);
	SDL_RenderDrawLine(gRenderer, currBlock.x, currBlock.y, (int) (relativePos.x * gXPixelsPerUnit),
		(int) (relativePos.y * gYPixelsPerUnit));

	//END_DEBUG^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
}

void _GR_DrawCursor()
{
	int cursorSize = 10;
	Real2 pos = IN_GetMouseCameraPos();
	pos.x *= gXPixelsPerUnit;
	pos.y *= gYPixelsPerUnit;

	SDL_Rect cursorBlock = (SDL_Rect) {
				(int) pos.x -cursorSize/2,
				(int) pos.y -cursorSize/2,
				cursorSize,
				cursorSize,
	};

	SDL_RenderFillRect(gRenderer, &cursorBlock);

}

double GR_GetCameraWidth()
{
	return CA_GetWidth(GO_GetComponent(gCamera, COMP_CAMERA));
}

double GR_GetCameraHeight()
{
	return GR_GetCameraWidth() * (double) SCREEN_HEIGHT / (double) SCREEN_WIDTH;
}

SDL_Renderer *GR_GetMainRenderer(void)
{
	return gRenderer;
}
