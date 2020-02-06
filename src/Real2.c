#include "Real2.h"

Real2 R2_Add(Real2 u, Real2 v)
{
	return (Real2) { u.x + v.x, u.y + v.y };
}

Real2 R2_Sub(Real2 u, Real2 v)
{
	return (Real2) { u.x - v.x, u.y - v.y };
}

Real2 R2_ScalarMult(Real2 u, double c)
{
	return (Real2) { c * u.x, c * u.y };
}

double R2_Dot(Real2 u, Real2 v)
{
	return u.x * v.x + u.y * v.y;
}

double R2_Mag(Real2 u)
{
	return sqrt(R2_Dot(u, u));
}

double R2_Dist(Real2 u, Real2 v)
{
	return R2_Mag(R2_Sub(u, v));
}

double R2_DistSq(Real2 u, Real2 v)
{
	Real2 sub;
	sub = R2_Sub(u, v);
	return R2_Dot(sub, sub);
}

Real2 R2_Norm(Real2 u)
{
	double mag = R2_Mag(u);
	if (mag != 0)
		return (Real2) { u.x/mag, u.y/mag };
	else
		return (Real2) { 0, 0 };
}

Real2 R2_RotateRad(Real2 u, double angle)
{
	double sintheta, costheta;

	sintheta = sin(angle);
	costheta = cos(angle);
	return (Real2) { 
		u.x * costheta - u.y * sintheta,
		u.x * sintheta + u.y * costheta
	};
}

Real2 R2_RotateDeg(Real2 u, double angle)
{
	return R2_RotateRad(u, angle * M_PI / 180);
}

Real2 R2_Normal(Real2 u)
{
	return (Real2) {-u.y, u.x};
}
