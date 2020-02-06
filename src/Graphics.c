#include "Graphics.h"

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;

// Local procedure declarations
double _GR_GetCameraHeight(Object *camera);
double _GR_GetCameraWidth(Object *camera);
void _GR_RenderShip(Object *ship, Object *camera, Transform relativeTransform);
void _GR_RenderTrail(Object *trail, Object *camera);
void _GR_RenderPolygonCollider(Object *obj, Object *camera);
void _GR_RenderRec(Object *root);

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

	_GR_RenderRec(root);
	SDL_RenderPresent(gRenderer);
}

void _GR_RenderRec(Object *root)
{
	int xWithinRange, yWithinRange;
	Object *camera = GO_GetCamera();
	List *children;
	Transform rootGlobalTransform, cameraGlobalTransform, relativeTransform;
	if (root == NULL) {
		fprintf(stderr, "Cannot render a null root\n");
		exit(1);
	}
	children = root->children;
	
	// Get global positions and check if object is in camera view
	rootGlobalTransform = GO_GetRootTransform(root);
	cameraGlobalTransform = GO_GetRootTransform(camera);
	relativeTransform = GO_GetRelativeTransform_T(&cameraGlobalTransform, &rootGlobalTransform);
	xWithinRange = relativeTransform.pos.x >= 0 && relativeTransform.pos.x <= _GR_GetCameraWidth(camera);
	yWithinRange = relativeTransform.pos.y >= 0 && relativeTransform.pos.y <= _GR_GetCameraHeight(camera);


	// Render if in view
	if (xWithinRange && yWithinRange) {
		switch (root->type) {
			case OBJ_SHIP:
				_GR_RenderShip(root, camera, relativeTransform);
				break;
			case OBJ_TRAIL:
				_GR_RenderTrail(root, camera);
				break;
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
	double xPixelsPerUnit, yPixelsPerUnit;
	int count, i, length, oldest;
	Trail *trailObj;
	Real2 localPointPos, prevPos;

	xPixelsPerUnit = SCREEN_WIDTH / _GR_GetCameraWidth(camera);
	yPixelsPerUnit = SCREEN_HEIGHT / _GR_GetCameraHeight(camera);

	trailObj = (Trail *) trail->obj;
	length = trailObj->length;
	oldest = i = trailObj->next;
	localPointPos = GO_GetLocalPositionTo(camera, trailObj->points[oldest]);
	for (count = 0; count < length; i = (i + 1) % length, count++) {
		prevPos = localPointPos;
		localPointPos = GO_GetLocalPositionTo(camera, trailObj->points[i]);
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
		SDL_RenderDrawLine(
				gRenderer, (int) (prevPos.x * xPixelsPerUnit),
				(int) (prevPos.y * yPixelsPerUnit),
				(int) (localPointPos.x * xPixelsPerUnit),
				(int) (localPointPos.y * yPixelsPerUnit)
				);
		/*printf("DEBUG: Rendering trail %s\n", trail->name);*/
	}
}

void _GR_RenderPolygonCollider(Object *obj, Object *camera)
{
	double xPixelsPerUnit, yPixelsPerUnit;
	List *currVertex;
	Real2 localPointPos, prevPos, firstPos;

	xPixelsPerUnit = SCREEN_WIDTH / _GR_GetCameraWidth(camera);
	yPixelsPerUnit = SCREEN_HEIGHT / _GR_GetCameraHeight(camera);

	currVertex = ((Polygon *) obj->collider.collider)->vertices;
	firstPos = localPointPos = GO_GetLocalPositionTo(camera, GO_GetRootPositionFrom(obj, *((Real2 *)List_Head(currVertex))));
	while (currVertex != NULL) {
		prevPos = localPointPos;
		localPointPos = GO_GetLocalPositionTo(camera, GO_GetRootPositionFrom(obj, *((Real2 *)List_Head(currVertex))));
		currVertex = List_Tail(currVertex);
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
		SDL_RenderDrawRect(gRenderer, &(SDL_Rect) {
				(int) (localPointPos.x * xPixelsPerUnit) - 2,
				(int) (localPointPos.y * yPixelsPerUnit) - 2,
				4,
				4,
				});
		SDL_RenderDrawLine(
				gRenderer, (int) (prevPos.x * xPixelsPerUnit),
				(int) (prevPos.y * yPixelsPerUnit),
				(int) (localPointPos.x * xPixelsPerUnit),
				(int) (localPointPos.y * yPixelsPerUnit)
				);
	}
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
	SDL_RenderDrawLine(
			gRenderer, (int) (localPointPos.x * xPixelsPerUnit),
			(int) (localPointPos.y * yPixelsPerUnit),
			(int) (firstPos.x * xPixelsPerUnit),
			(int) (firstPos.y * yPixelsPerUnit)
			);
}

void _GR_RenderShip(Object *ship, Object *camera, Transform relativeTransform)
{
	//TODO: Figure out how to optimize by minimizing floating point operations
	//TODO: Block rendering seems somewhat unstable. Try using rotated basis instead
	double xPixelsPerUnit, yPixelsPerUnit;
	int x, y, shipWidth, shipHeight;
	Block *block;
	SDL_Rect currBlock;
	SDL_Point centerOfRotation;
	Real2 rotatedBlockPos;
	Real2 rotatedBasisX, rotatedBasisY;
	
	centerOfRotation = (SDL_Point) { 0, 0 };
	xPixelsPerUnit = SCREEN_WIDTH / _GR_GetCameraWidth(camera);
	yPixelsPerUnit = SCREEN_HEIGHT / _GR_GetCameraHeight(camera);
	shipWidth = ((Ship *) ship->obj)->width;
	shipHeight = ((Ship *) ship->obj)->height;
	rotatedBasisX = R2_RotateDeg((Real2) {xPixelsPerUnit, 0}, relativeTransform.rot);
	rotatedBasisY = R2_RotateDeg((Real2) {0, yPixelsPerUnit}, relativeTransform.rot);

	for (y = 0; y < shipHeight; y++) {
		for (x = 0; x < shipWidth; x++) {
			block = GO_ShipGetBlock(ship, x, y);
			rotatedBlockPos = R2_RotateDeg((Real2) { x, y }, relativeTransform.rot);
			currBlock = (SDL_Rect) {
				(int) ((relativeTransform.pos.x + rotatedBlockPos.x) * xPixelsPerUnit),
				(int) ((relativeTransform.pos.y + rotatedBlockPos.y) * yPixelsPerUnit),
				(int) ceil(xPixelsPerUnit),
				(int) ceil(yPixelsPerUnit),
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

	//TODO: DEBUG UGLY STUFF. DELETE OR FIX vvvvvvvvvv
	rotatedBlockPos = R2_RotateDeg(ship->physics.centerOfMass, relativeTransform.rot);
	currBlock = (SDL_Rect) {
				(int) ((relativeTransform.pos.x + rotatedBlockPos.x) * xPixelsPerUnit) -2,
				(int) ((relativeTransform.pos.y + rotatedBlockPos.y) * yPixelsPerUnit) -2,
				4,
				4,
	};

	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
	SDL_RenderDrawRect(gRenderer, &currBlock);
	SDL_RenderDrawLine(gRenderer, currBlock.x, currBlock.y, (int) (relativeTransform.pos.x * xPixelsPerUnit),
		(int) (relativeTransform.pos.y * yPixelsPerUnit));

	_GR_RenderPolygonCollider(ship, camera);
	//END_DEBUG^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

}

double _GR_GetCameraWidth(Object *camera)
{
	return ((Camera *) camera->obj)->width;
}

double _GR_GetCameraHeight(Object *camera)
{
	return _GR_GetCameraWidth(camera) * (double) SCREEN_HEIGHT / (double) SCREEN_WIDTH;
}

SDL_Renderer *GR_GetMainRenderer(void)
{
	return gRenderer;
}
