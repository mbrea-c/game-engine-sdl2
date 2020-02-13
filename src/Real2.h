#ifndef REAL2_H
#define REAL2_H

#include <math.h>
#include <stdio.h>

/* In case M_PI is not defined */
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif /*M_PI*/

typedef struct Real2 Real2;

struct Real2 {
	double x;
	double y;
};

Real2 R2_Add(Real2 u, Real2 v);
Real2 R2_Sub(Real2 u, Real2 v);
Real2 R2_ScalarMult(Real2 u, double c);
double R2_Dot(Real2 u, Real2 v);
double R2_Dist(Real2 u, Real2 v);
double R2_DistSq(Real2 u, Real2 v);
double R2_Mag(Real2 u);
double R2_AngleDeg(Real2 u);
double R2_ProjectOn(Real2 u, Real2 projectionTarget);
double R2_ProjectOnUnit(Real2 u, Real2 unitProjectionTarget);
Real2 R2_Norm(Real2 u);
Real2 R2_RotateDeg(Real2 u, double angle);
Real2 R2_RotateRad(Real2 u, double angle);
Real2 R2_Normal(Real2 u); //Anticlockwise

#endif //REAL2_H
