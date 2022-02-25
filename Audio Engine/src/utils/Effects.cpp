#include <xaudio2_engine/utils/Effects.h>

#include <xaudio2_engine/utils/math.h>

/// <summary>
/// Changes the value of a byte.
/// </summary>
/// <param name="value">The byte.</param>
/// <param name="volume">The volume.</param>
/// <returns></returns>
int16_t ChangeByteVolume(int16_t value, float volume)
{
	float converted_value = static_cast<float>(value);
	converted_value *= volume;

	return static_cast<int16_t>(converted_value);
}

/// <summary>
/// Changes the volume of pcm data.
/// </summary>
/// <param name="a_Data">The actual data.</param>
/// <param name="a_Size">The data size.</param>
/// <param name="a_Volume">The volume.</param>
/// <returns></returns>
unsigned char* effects::ChangeVolume(unsigned char* a_Data, uint32_t a_Size, float a_Volume)
{
	// Clamp the volume to 0.0 min and 1.0 max.
	a_Volume = math::ClampF(a_Volume, 0.0f, 1.0f);

	int16_t* array_16 = reinterpret_cast<int16_t*>(a_Data);
	for (uint32_t i = 0; i < a_Size; i++)
		array_16[i] = ChangeByteVolume(array_16[i], a_Volume);

	return reinterpret_cast<unsigned char*>(array_16);
}

/// <summary>
/// Changes the volume of pcm data.
/// </summary>
/// <param name="a_Data">The actual data.</param>
/// <param name="a_Size">The data size.</param>
/// <param name="a_Amount">The panning amount (-1 is fully left, 1 is fully right, 0 is middle).</param>
/// <returns></returns>
unsigned char* effects::ChangePanning(unsigned char* a_Data, uint32_t a_Size, float a_Amount, uint16_t a_NumChannels)
{
	if (a_NumChannels == 1)
		return a_Data;

	// Amount is a value from -1 to 1.
	a_Amount = math::ClampF(a_Amount, -1.0f, 1.0f);

	// Set the values to 1.0 as default.
	float left = 1.0f, right = 1.0f;

	// If the slider is more to the left.
	if (a_Amount < 0)
	{
		right += a_Amount;
		// Clamp the volume to 0.0 min and 1.0 max.
		right = math::ClampF(right, 0.0f, 1.0f);
	}
	// If the slider is more to the right.
	else if (a_Amount > 0)
	{
		left -= a_Amount;
		// Clamp the volume to 0.0 min and 1.0 max.
		left = math::ClampF(left, 0.0f, 1.0f);
	}

	int16_t* array_16 = reinterpret_cast<int16_t*>(a_Data);
	for (uint32_t i = 0; i < a_Size; i += a_NumChannels)
	{
		array_16[i] = ChangeByteVolume(array_16[i], left);
		array_16[i + 1] = ChangeByteVolume(array_16[i + 1], right);
	}

	return reinterpret_cast<unsigned char*>(array_16);
}