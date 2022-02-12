#include "Audio/math.h"

#include <algorithm>

namespace math
{
	float fclamp(float value, float min, float max)
	{
		return std::max(min, std::min(value, max));
	}
}
