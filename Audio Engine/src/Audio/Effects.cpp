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
	return data;
}