#ifndef SEGMENT_H
#define SEGMENT_H

#include "Real2.h"

typedef struct Segment Segment;
typedef struct Intersection Intersection;

struct Segment {
	Real2 start;
	Real2 end;
};

struct Intersection {
	int didIntersect; // Was there an intersection?
	Real2 intersection;
};

Segment *SG_Allocate(Real2 start, Real2 end);
void SG_SetStart(Segment *segment, Real2 start);
void SG_SetEnd(Segment *segment, Real2 end);
Intersection SG_Intersect(Segment segment0, Segment segment1);
void SG_Destroy(Segment *segment);

#endif //SEGMENT_H
