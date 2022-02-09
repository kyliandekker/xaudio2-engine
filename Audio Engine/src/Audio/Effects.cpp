#include <iostream>

#include <Windows.h>

#include "Audio/WavConverter.h"

#include "Audio/Effects.h"

int16_t change_byte_volume(int16_t value, float volume)
{
	float converted_value = static_cast<float>(value);
	converted_value *= volume;

	return static_cast<int16_t>(converted_value);
}

unsigned char* effects::change_volume(unsigned char* data, uint32_t size, float volume)
{
	int16_t* array_16 = reinterpret_cast<int16_t*>(data);
	for (uint32_t i = 0; i < size; i++)
		array_16[i] = change_byte_volume(array_16[i], volume);

	return reinterpret_cast<unsigned char*>(array_16);
}

unsigned char* effects::change_panning(unsigned char* data, uint32_t size, float amount)
{
	float left = 1.0f, right = 1.0f;
	if (amount < 0)
	{
		amount = abs(amount);
		left -= amount;
		right += amount;
	}
	else if (amount > 0)
	{
		amount = abs(amount);
		right -= amount;
		left += amount;
	}
	if (right > 1)
		right = 1;
	if (left > 1)
		left = 1;
	if (right < 0)
		right = 0;
	if (left < 0)
		left = 0;

	int16_t* array_16 = reinterpret_cast<int16_t*>(data);
	for (uint32_t i = 0; i < size; i += 2)
	{
		array_16[i] = change_byte_volume(array_16[i], right);
		array_16[i + 1] = change_byte_volume(array_16[i + 1], left);
	}

	return reinterpret_cast<unsigned char*>(array_16);
}