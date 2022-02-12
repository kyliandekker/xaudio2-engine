#include <Audio/Effects.h>

#include <Audio/math.h>

int16_t change_byte_volume(int16_t value, float volume)
{
	float converted_value = static_cast<float>(value);
	converted_value *= volume;

	return static_cast<int16_t>(converted_value);
}

unsigned char* effects::change_volume(unsigned char* data, uint32_t size, float volume)
{
	// Clamp the volume to 0.0 min and 1.0 max.
	volume = math::fclamp(volume, 0.0f, 1.0f);

	int16_t* array_16 = reinterpret_cast<int16_t*>(data);
	for (uint32_t i = 0; i < size; i++)
		array_16[i] = change_byte_volume(array_16[i], volume);

	return reinterpret_cast<unsigned char*>(array_16);
}

unsigned char* effects::change_panning(unsigned char* data, uint32_t size, float amount)
{
	// Amount is a value from -1 to 1.
	amount = math::fclamp(amount, -1.0f, 1.0f);

	// Set the values to 1.0 as default.
	float left = 1.0f, right = 1.0f;

	// If the slider is more to the left.
	if (amount < 0)
	{
		right += amount;
		// Clamp the volume to 0.0 min and 1.0 max.
		right = math::fclamp(right, 0.0f, 1.0f);
	}
	// If the slider is more to the right.
	else if (amount > 0)
	{
		left -= amount;
		// Clamp the volume to 0.0 min and 1.0 max.
		left = math::fclamp(left, 0.0f, 1.0f);
	}

	int16_t* array_16 = reinterpret_cast<int16_t*>(data);
	for (uint32_t i = 0; i < size; i += 2)
	{
		array_16[i] = change_byte_volume(array_16[i], left);
		array_16[i + 1] = change_byte_volume(array_16[i + 1], right);
	}

	return reinterpret_cast<unsigned char*>(array_16);
}