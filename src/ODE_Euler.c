#include "ODE_Euler.h"

#define PHYSICS_TIMESTEP 1.0/60.0 //In seconds

void euler_step(ODEVector y, double t, double dt, dydt_funct dydt);

void ode(ODEVector y, double t0, double t1, dydt_funct dydt)
{
	int step, nTimesteps;
	double deltaT, lastTimestep;

	deltaT = t1 - t0;
	nTimesteps = (int) (deltaT / PHYSICS_TIMESTEP);
	lastTimestep = deltaT - nTimesteps * PHYSICS_TIMESTEP;


	for (step = 0; step < nTimesteps; step++) {
		euler_step(y, t0 + step * PHYSICS_TIMESTEP, PHYSICS_TIMESTEP, dydt);
	}
	if (lastTimestep > 0) {
		euler_step(y, t0 + step * PHYSICS_TIMESTEP, PHYSICS_TIMESTEP, dydt);
	}
	y.updateVals(y.objects);
}

void euler_step(ODEVector y, double t, double dt, dydt_funct dydt)
{
	int i, dim;
	dim = y.getDim(y.objects);
	for (i = 0; i < dim; i++) {
		y.setNextVal(y.objects, i, y.getVal(y.objects, i) + dt * dydt(t, y, i));
	}
}
