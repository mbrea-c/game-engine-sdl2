#ifndef INTERVAL_H
#define INTERVAL_H

#include <stddef.h>

typedef struct Interval Interval;

struct Interval {
	double start;
	double end;
};

int IN_IsInInterval(Interval *interval, double val);
int IN_IsIntersecting(Interval *interval1, Interval *interval2);

#endif //INTERVAL_H
