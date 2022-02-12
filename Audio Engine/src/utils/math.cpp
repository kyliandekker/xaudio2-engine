#include <xaudio2_engine/utils/math.h>

#include <algorithm>

namespace math
{
	/// <summary>
	/// Clamps a number between a min and max value.
	/// </summary>
	/// <param name="a_Value">The number.</param>
	/// <param name="a_Min">The min value.</param>
	/// <param name="a_Max">The max value.</param>
	/// <returns></returns>
	float ClampF(float a_Value, float a_Min, float a_Max)
	{
		return std::max(a_Min, std::min(a_Value, a_Max));
	}
}
