#include <assert.h>
#include "Real2.h"

int main(int argc, char **argv)
{
	Real2 u, v;
	u = (Real2) {2, 3};
	v = (Real2) {-1, 4};

	// Check addition
	Real2 added = R2_Add(u, v);
	assert(added.x == 1 && added.y == 7);

	// Check magnitude of norm is 1
	assert(R2_Mag(R2_Norm(v)) == 1.0);

	exit(0);
}
