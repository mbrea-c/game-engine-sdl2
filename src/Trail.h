#ifndef TRAIL_H
#define TRAIL_H

#include "Typedefs.h"
#include "Real2.h"

// Basic
Component *TL_Create(int length, RGBA color);
void TL_Mount(Component *trail);
void TL_Destructor(void *trailData);

// Getters
int TL_GetLength(Component *trail);
int TL_GetNext(Component *trail);
Real2 *TL_GetPoints(Component *trail);
Real2 TL_GetPoint(Component *trail, int index);
RGBA TL_GetColor(Component *trail);

// Setters
void TL_SetColor(Component *trail, RGBA color);

// Trail specific procedures
void TL_PushToTrail(Component *trail);

#endif //TRAIL_H
