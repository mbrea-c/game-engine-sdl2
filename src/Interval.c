#include "Interval.h"

int IN_IsInInterval(Interval *interval, double val)
{
	return interval != NULL && val >= interval->start && val <= interval->end;
}

//TODO: What even is this mess?
int IN_IsIntersecting(Interval *interval1, Interval *interval2)
{
	if (interval1 == NULL || interval2 == NULL) return 0;
	
	if (IN_IsInInterval(interval1, interval2->start)
		|| IN_IsInInterval(interval1, interval2->end)
		|| IN_IsInInterval(interval2, interval1->start)
		|| IN_IsInInterval(interval2, interval1->end))
		return 1;
	else return 0;
}
