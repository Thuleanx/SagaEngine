#include "line.h"

namespace Saga::Geometry {
	float detectLineSegmentCollision(
		const float &alo, const float &ahi, 
		const float &blo, const float &bhi) {

		float aUp = bhi - alo, bDown = ahi - blo;
		if (aUp < 0 || bDown < 0) return 0;
		if (aUp < bDown) return aUp;
		return -bDown;
	}
}
