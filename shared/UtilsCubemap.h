#pragma once

#include <glm/glm.hpp>

#include "shared/Bitmap.h"

Bitmap convertEquirectangularMapToVerticalCross(const Bitmap& b);
Bitmap convertVerticalCrossToCubeMapFaces(const Bitmap& b);

inline Bitmap convertEquirectangularMapToCubeMapFaces(const Bitmap& b) {
	return convertVerticalCrossToCubeMapFaces(convertEquirectangularMapToVerticalCross(b));
}
