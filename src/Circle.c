#include "Circle.h"

Circle *CI_Allocate(Real2 center, double radius)
{
	Circle *circle = malloc(sizeof(circle));
	circle->center = center;
	circle->radius = radius;
	return circle;
}

void *CI_SetCenter(Circle *circle, Real2 center)
{
	circle->center = center;
}

void *CI_SetRadius(Circle *circle, double radius)
{
	circle->radius = radius;
}

void *CI_Destroy(Circle *circle)
{
	free(circle);
}
