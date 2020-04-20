#ifndef ODE_EULER_H
#define ODE_EULER_H

#include <stdio.h>

typedef struct ODEVector ODEVector;
typedef double (*dydt_funct)(double t, ODEVector y, int ind);

struct ODEVector {
	// Pointers to procedures that access the vector
	int    (*getDim)(void);
	void   (*setNextVal)(int i, double newVal);
	void   (*updateVals)(void);
	double (*getVal)(int i);
};

void ode(ODEVector y, double t0, double t1, dydt_funct dydt);

#endif //ODE_EULER_H
