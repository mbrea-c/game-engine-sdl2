#include "Segment.h"
#include <math.h>

#define INTERSECT_EPSILON 0.0001

Segment *SG_Allocate(Real2 start, Real2 end)
{
	Segment *segment = malloc(sizeof(Segment));
	segment->start = start;
	segment->end = end;
	return segment;
}

void SG_SetStart(Segment *segment, Real2 start)
{
	segment->start = start;
}

void SG_SetEnd(Segment *segment, Real2 end)
{
	segment->end = end;
}

Intersection SG_Intersect(Segment segment0, Segment segment1)
{
	// Credit for this approach goes to
	// https://www.codeproject.com/Articles/5252711/Magic-Formula-of-the-Intersection-Point-of-Two-Lin

	Real2 r0, r1; // direction vectors
	r0 = R2_Sub(segment0.end, segment0.start);
	r1 = R2_Sub(segment1.end, segment1.start);

	// Test if segments are parallel
	int segmentsParallel;
	segmentsParallel = fabs(R2_Wedge(r0, r1)) / (R2_Mag(r0) * R2_Mag(r1)) <= INTERSECT_EPSILON;
	// Compute intersection if segments not parallel
	Real2 intersection = R2_ZERO;
	int didIntersect = 0;
	if (!segmentsParallel) {
		intersection = 
			R2_ScalarMult(R2_Sub(
				R2_ScalarMult(r0, R2_Wedge(segment1.end, segment1.start)), 
				R2_ScalarMult(r1, R2_Wedge(segment0.end, segment0.start))),
			1.0/R2_Wedge(r1, r0));
		didIntersect = 
			(R2_Dot(R2_Sub(intersection, segment0.start), R2_Sub(intersection, segment0.end)) <= 0) && 
			(R2_Dot(R2_Sub(intersection, segment1.start), R2_Sub(intersection, segment1.end)) <= 0);
	} else {
		int segmentsCollinear;
		segmentsCollinear = 
			fabs(R2_Wedge(r1, R2_Sub(segment1.start, segment0.start)))
			/ (R2_Mag(r1) * R2_Mag(R2_Sub(segment1.start, segment0.start)))
			<= INTERSECT_EPSILON;
		if (segmentsCollinear) {
			// Rule out non-collision
			double r00r10_r00r11, r01r10_r01r11, r10r00_r10r01, r11r00_r11r01; 
			r00r10_r00r11 = R2_Dot(R2_Sub(segment0.start, segment1.start), R2_Sub(segment0.start, segment1.end));
			r01r10_r01r11 = R2_Dot(R2_Sub(segment0.end, segment1.start), R2_Sub(segment0.end, segment1.end));
			r10r00_r10r01 = R2_Dot(R2_Sub(segment1.start, segment0.start), R2_Sub(segment1.start, segment0.end));
			r11r00_r11r01 = R2_Dot(R2_Sub(segment1.end, segment0.start), R2_Sub(segment1.end, segment0.end));
			// Segments overlap
			if (!(r00r10_r00r11 > 0 && r01r10_r01r11 > 0 && r10r00_r10r01 > 0 && r11r00_r11r01 > 0)) {
				didIntersect = 1;
				// Segments in same direction
				if (R2_Dot(r1, r0) > 0) {
					if (r00r10_r00r11 < 0 && r01r10_r01r11 < 0) {
						intersection = R2_ScalarMult(R2_Add(segment0.start, segment0.end), 0.5);
					} else if (r00r10_r00r11 < 0 && r01r10_r01r11 > 0) {
						intersection = R2_ScalarMult(R2_Add(segment0.start, segment1.end), 0.5);
					} else if (r00r10_r00r11 > 0 && r11r00_r11r01 < 0) {
						intersection = R2_ScalarMult(R2_Add(segment1.start, segment1.end), 0.5);
					} else if (r00r10_r00r11 > 0 && r01r10_r01r11 < 0) {
						intersection = R2_ScalarMult(R2_Add(segment1.start, segment0.end), 0.5);
					}
				}
				// Segments in opposite direction
				else {
					if (r00r10_r00r11 < 0 && r10r00_r10r01 > 0) {
						intersection = R2_ScalarMult(R2_Add(segment0.start, segment0.end), 0.5);
					} else if (r00r10_r00r11 < 0 && r10r00_r10r01 < 0) {
						intersection = R2_ScalarMult(R2_Add(segment0.start, segment1.start), 0.5);
					} else if (r00r10_r00r11 > 0 && r10r00_r10r01 < 0) {
						intersection = R2_ScalarMult(R2_Add(segment1.end, segment1.start), 0.5);
					} else if (r00r10_r00r11 > 0 && r01r10_r01r11 < 0) {
						intersection = R2_ScalarMult(R2_Add(segment1.end, segment0.end), 0.5);
					}
				}
			}
		}
	}
	return (Intersection) {didIntersect, intersection};
}

void SG_Destroy(Segment *segment)
{
	free(segment);
}
