#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Real2.h"
#include "Typedefs.h"

// Basic
Component *TR_CreateTransform(Real2 pos, double rot);
Component *TR_GetFromObj(Object *object);

// Getters
Real2 TR_GetPos(Component *transform);
double TR_GetRot(Component *transform);
// Setters
void TR_SetPos(Component *transform, Real2 pos);
void TR_SetRot(Component *transform, double rot);

//given obj O and local space pos P, return P in local space of O's parent
Real2 TR_PosToParentSpace(Component *transform, Real2 localPos);
//given obj O and local space pos P, return P in root space
Real2 TR_PosToRootSpace(Component *transform, Real2 localPos);
//given obj O and root space pos P, return P in local space of O
Real2 TR_PosToLocalSpace(Component *transform, Real2 rootPos);
Real2 TR_PosToRootSpaceObj(Component *transform);
double TR_RotToParentSpace(Component *transform, double localRot);
double TR_RotToRootSpace(Component *transform, double localRot);
double TR_RotToLocalSpace(Component *transform, double rootRot);
double TR_RotToRootSpaceObj(Component *transform);
//Transform TR_TransformToParentSpace(Component *transform, Transform localTransform);
//Transform TR_TransformToRootSpace(Component *transform, Transform localTransform);
//Transform TR_TransformToLocalSpace(Component *transform, Transform rootTransform);
//Transform TR_TransformToRootSpaceObj(Component *transform);

#endif //TRANSFORM_H
