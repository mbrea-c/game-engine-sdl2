#ifndef ODE_EULER_H
#define ODE_EULER_H

#include <stdio.h>

typedef struct ODEVector ODEVector;
typedef double (*dydt_funct)(double t, ODEVector y, int ind);

struct ODEVector {
	// Pointers to procedures that access the vector
	int    (*getDim)(void *objects);
	void   (*setNextVal)(void *objects, int i, double newVal);
	void   (*updateVals)(void *objects);
	double (*getVal)(void *objects, int i);
	// Pointer to actual objects
	void *objects;
};

void ode(ODEVector y, double t0, double t1, dydt_funct dydt);

#endif //ODE_EULER_H
