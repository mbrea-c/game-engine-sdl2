#ifndef TESTMOUSEFORCE_H
#define TESTMOUSEFORCE_H

#include "Typedefs.h"
#include "Real2.h"

// Basic
Component *TMF_Create(Real2 localDragPoint, double springConstant);

// Getters
Real2 TMF_GetLocalDragPoint(Component *testMouseForce);
double TMF_GetSpringConstant(Component *testMouseForce);

// Setters
void TMF_SetLocalDragPoint(Component *testMouseForce, Real2 localDragPoint);

#endif //TESTMOUSEFORCE_H
