#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <unistd.h>
#include "LTexture.h"
#include "List.h"
#include "Graphics.h"
#include "GameObject.h"
#include "Transform.h"
#include "Assets.h"
#include "Trail.h"
#include "Physics.h"
#include "Collider.h"
#include "Camera.h"
#include "Ship.h"
#include "Input.h"
#include "Segment.h"
#include "Circle.h"
#include "Debug.h"
#include "Engine.h"
#include "TestMover.h"
#include "TestMouseForce.h"

#define FPS_CAP 60
#define TICKS_PER_FRAME 1000 / FPS_CAP

Real2 springForce(Real2 x, double k);

int main(int argc, char **argv)
{
	int quit, willUpdate, stepByStep;
	double timeCoefficient;
	Uint32 startTime, frameTime;
	SDL_Event event;
	Object *root = GO_CreateObject("top", 0, 0, 0, NULL);

	// TODO: Investigate why camera can't be child of ship (segfault)
	Object *camera = GO_CreateObject("camerita", -5, -5, 0, root);
	Component *cameraComponent = CA_Create(20);
	Component *cameraTransform = GO_GetComponent(camera, COMP_TRANSFORM);
	printf("DEBUG: Adding Camera component\n");
	GO_AddComponent(camera, cameraComponent);
	GR_Init(camera);

	Object *ship = GO_CreateObject("shipi", 0, 0, 0, root); 
	GO_AddComponent(ship, PH_CreatePhysicsZeroed());
	GO_AddComponent(ship, CD_CreateCollider(COLL_POLYGON));
	GO_AddComponent(ship, SH_CreateShipEmpty(10, 15));
	Component *shipComp = GO_GetComponent(ship, COMP_SHIP);
	Component *shipPhysics = GO_GetComponent(ship, COMP_PHYSICS);

	Object *ship2 = GO_CreateObject("shipi_drone", 4, 4, 0, ship); 
	GO_AddComponent(ship2, PH_CreatePhysicsZeroed());
	GO_AddComponent(ship2, CD_CreateCollider(COLL_POLYGON));
	GO_AddComponent(ship2, SH_CreateShipEmpty(10, 15));
	Component *ship2Comp = GO_GetComponent(ship2, COMP_SHIP);
	SH_SetBlock(ship2Comp, 0,0, SH_CreateUnwalledBlock(BLOCK_TEST));
	SH_SetBlock(ship2Comp, 1,0, SH_CreateUnwalledBlock(BLOCK_TEST));

	SH_UpdatePhysicsData(ship2Comp);
	printf("DEBUG: Updated ship physics data\n");
	SH_CloseWithWalls(ship2Comp, WALL_LIGHT);
	printf("DEBUG: Closed off ship walls\n");
	SH_UpdateCollider(ship2Comp);
	printf("DEBUG: Updated ship collider\n");

	printf("DEBUG: Got to the end of ship creation\n");

	Object *shipTrail0 = GO_CreateObject("trail0", 0, 0, 0, ship);
	GO_AddComponent(shipTrail0, TL_Create(100, (RGBA) {255, 0, 0, 255}));
	Object *shipTrail1 = GO_CreateObject("trail1", 5, 0, 0, ship);
	GO_AddComponent(shipTrail1, TL_Create(100, (RGBA) {255, 0, 0, 255}));
	Object *shipTrail2 = GO_CreateObject("trail2", 5, 1, 0, ship);
	GO_AddComponent(shipTrail2, TL_Create(100, (RGBA) {255, 0, 0, 255}));
	Object *shipTrail3 = GO_CreateObject("trail3", 0, 1, 0, ship);
	GO_AddComponent(shipTrail3, TL_Create(100, (RGBA) {255, 0, 0, 255}));

	Object *ship2Trail0 = GO_CreateObject("trail20", 0, 0, 0, ship2);
	GO_AddComponent(ship2Trail0, TL_Create(100, (RGBA) {0, 255, 255, 255}));
	Object *ship2Trail1 = GO_CreateObject("trail21", 2, 0, 0, ship2);
	GO_AddComponent(ship2Trail1, TL_Create(100, (RGBA) {0, 255, 255, 255}));
	Object *ship2Trail2 = GO_CreateObject("trail22", 2, 1, 0, ship2);
	GO_AddComponent(ship2Trail2, TL_Create(100, (RGBA) {0, 255, 255, 255}));
	Object *ship2Trail3 = GO_CreateObject("trail23", 0, 1, 0, ship2);
	GO_AddComponent(ship2Trail3, TL_Create(100, (RGBA) {0, 255, 255, 255}));

	SH_SetBlock(shipComp, 0,0, SH_CreateUnwalledBlock(BLOCK_TEST));
	SH_SetBlock(shipComp, 1,0, SH_CreateUnwalledBlock(BLOCK_TEST));
	SH_SetBlock(shipComp, 2,0, SH_CreateUnwalledBlock(BLOCK_TEST));
	SH_SetBlock(shipComp, 3,0, SH_CreateUnwalledBlock(BLOCK_TEST));
	SH_SetBlock(shipComp, 4,0, SH_CreateUnwalledBlock(BLOCK_TEST));

	SH_CloseWithWalls(shipComp, WALL_LIGHT);
	

	PH_SetLinearVelocity(shipPhysics, (Real2) {5, -2});

	// COLLIDER TESTS
	/*Object *segmentColliderTest = GO_CreateObject("segmentColliderTest", 0, 0, 0, root); */
	/*GO_AddComponent(segmentColliderTest, PH_CreatePhysicsZeroed());*/
	/*Component *segmentColliderTestCollider = CD_CreateCollider(COLL_SEGMENT);*/
	/*SG_SetStart(CD_GetCollider(segmentColliderTestCollider), (Real2) {0, 0});*/
	/*SG_SetEnd(CD_GetCollider(segmentColliderTestCollider), (Real2) {5, 0});*/
	/*GO_AddComponent(segmentColliderTest, segmentColliderTestCollider);*/
	/*GO_AddComponent(segmentColliderTest, TM_Create());*/

	/*Object *segmentColliderTest2 = GO_CreateObject("segmentColliderTest2", 2, -2, 0, root); */
	/*GO_AddComponent(segmentColliderTest2, PH_CreatePhysicsZeroed());*/
	/*Component *segmentColliderTest2Collider = CD_CreateCollider(COLL_SEGMENT);*/
	/*SG_SetStart(CD_GetCollider(segmentColliderTest2Collider), (Real2) {0, 0});*/
	/*SG_SetEnd(CD_GetCollider(segmentColliderTest2Collider), (Real2) {1, 5});*/
	/*GO_AddComponent(segmentColliderTest2, segmentColliderTest2Collider);*/

	/*Object *circleColliderTest = GO_CreateObject("circleColliderTest", -2, 5, 0, root); */
	/*GO_AddComponent(circleColliderTest, PH_CreatePhysicsZeroed());*/
	/*Component *circleColliderTestCollider = CD_CreateCollider(COLL_CIRCLE);*/
	/*CI_SetCenter(CD_GetCollider(circleColliderTestCollider), (Real2) {0, 0});*/
	/*CI_SetRadius(CD_GetCollider(circleColliderTestCollider), 2.5);*/
	/*GO_AddComponent(circleColliderTest, circleColliderTestCollider);*/

	Component *shipTestMouseForce = TMF_Create(R2_ZERO, 50);
	GO_AddComponent(ship, shipTestMouseForce);
	

	// Main loop
	EN_RunEngine(root);

	return 0;
}

Real2 springForce(Real2 x, double k)
{
	return R2_ScalarMult(x,-k);
}
