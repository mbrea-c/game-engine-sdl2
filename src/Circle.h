#ifndef CIRCLE_H
#define CIRCLE_H

#include "Real2.h"

typedef struct Circle Circle;

struct Circle {
	Real2 center;
	double radius;
};

Circle *CI_Allocate(Real2 center, double radius);
void *CI_SetCenter(Circle *circle, Real2 center);
void *CI_SetRadius(Circle *circle, double radius);
void *CI_Destroy(Circle *circle);

#endif //CIRCLE_H
