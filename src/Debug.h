#ifndef DEBUG_H
#define DEBUG_H

// Anything that is only used for debugging and does not fit anywhere else
// goes here

#include "List.h"
#include "Polygon.h"

void Debug_PrintListIntPtrs(List *list);
void Debug_PrintPolygon(Polygon *polygon);
char *Debug_GetComponentTypeString(int type);

#endif //DEBUG_H
