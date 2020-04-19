#include "Transform.h"
#include "GameObject.h"
#include "Component.h"
#include "List.h"

// Basic
Component *TR_CreateTransform(Real2 pos, double rot)
{
	List *dependencies = List_Nil();
	TransformData *transformData = malloc(sizeof(TransformData));
	transformData->pos = pos;
	transformData->rot = rot;
	Component *component = CM_CreateComponent(COMP_TRANSFORM, transformData, &TR_Destructor, &TR_Mount, dependencies);
	return component;
}

void TR_Mount(Component *transform) {}

void TR_Destructor(void *transformData)
{
	free(transformData);
}

Component *TR_GetFromObj(Object *object)
{
	return GO_GetComponent(object, COMP_TRANSFORM);
}

// Getters
Real2 TR_GetPos(Component *transform)
{
	return ((TransformData *)transform->componentData)->pos;
}

double TR_GetRot(Component *transform)
{
	return ((TransformData *)transform->componentData)->rot;
}

// Setters
void TR_SetPos(Component *transform, Real2 pos)
{
	((TransformData *)transform->componentData)->pos = pos;
}

void TR_SetRot(Component *transform, double rot)
{
	((TransformData *)transform->componentData)->rot = rot;
}

// Transformations
Real2 TR_PosToParentSpace(Component *transform, Real2 localPos)
{
	return R2_Add(TR_GetPos(transform), R2_RotateDeg(localPos, TR_GetRot(transform)));
}

Real2 TR_PosToRootSpace(Component *transform, Real2 localPos)
{
	Object *obj = GO_GetComponentOwner(transform);
	while (obj->parent != NULL) {
		localPos = TR_PosToParentSpace(GO_GetComponent(obj, COMP_TRANSFORM), localPos);
		obj = obj->parent;
	}

	return localPos;
}

Real2 TR_PosToLocalSpace(Component *transform, Real2 rootPos)
{
	rootPos = R2_Sub(rootPos, TR_PosToRootSpaceObj(transform));
	rootPos = R2_RotateDeg(rootPos, -TR_RotToRootSpaceObj(transform));
	return rootPos;
}

Real2 TR_PosToRootSpaceObj(Component *transform)
{
	return TR_PosToRootSpace(transform, (Real2) {0, 0});
}

double TR_RotToParentSpace(Component *transform, double localRot)
{
	return localRot + TR_GetRot(transform);
}

double TR_RotToRootSpace(Component *transform, double localRot)
{
	Object *obj = GO_GetComponentOwner(transform);
	while (obj->parent != NULL) {
		localRot = TR_RotToParentSpace(GO_GetComponent(obj, COMP_TRANSFORM), localRot);
		obj = obj->parent;
	}

	return localRot;
}

double TR_RotToLocalSpace(Component *transform, double rootRot)
{
	return rootRot - TR_RotToRootSpace(transform, TR_GetRot(transform));
}

double TR_RotToRootSpaceObj(Component *transform)
{
	return TR_RotToRootSpace(transform, 0);
}

/*Transform TR_TransformToParentSpace(Component *transform, Transform localTransform)*/
/*{*/
	/*localTransform.pos = TR_PosToParentSpace(obj, localTransform.pos);*/
	/*localTransform.rot = TR_RotToParentSpace(obj, localTransform.rot);*/
	/*return localTransform;*/
/*}*/

/*Transform TR_TransformToRootSpace(Component *transform, Transform localTransform)*/
/*{*/
	/*localTransform.pos = TR_PosToRootSpace(obj, localTransform.pos);*/
	/*localTransform.rot = TR_RotToRootSpace(obj, localTransform.rot);*/
	/*return localTransform;*/
/*}*/

/*Transform TR_TransformToLocalSpace(Component *transform, Transform rootTransform)*/
/*{*/
	/*rootTransform.pos = TR_PosToLocalSpace(obj, rootTransform.pos);*/
	/*rootTransform.rot = TR_RotToLocalSpace(obj, rootTransform.rot);*/
	/*return rootTransform;*/
/*}*/

/*Transform TR_TransformToRootSpaceObj(Component *transform)*/
/*{*/
	/*return TR_TransformToRootSpace(obj, (Transform) {(Real2) {0, 0}, 0});*/
/*}*/
