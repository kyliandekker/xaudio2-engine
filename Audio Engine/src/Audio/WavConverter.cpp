#include <iostream>

#include "Audio/WavConverter.h"

namespace wav
{

	namespace wav_converter
	{
		uint16_t* convert_24_to_16(unsigned char* data, uint32_t size)
		{
			typedef struct { uint8_t byt[3]; } uint24_t;

			const uint32_t new_size = size / sizeof(uint24_t) * sizeof(uint16_t);
			uint16_t* array_16 = new uint16_t[new_size];

			for (uint32_t a = 0; a < new_size; a++)
			{
				// Skip 1 bit every time since we go from 24bit to 16bit.
				array_16[a] = *reinterpret_cast<uint16_t*>(data + 1);

				// Add the size of a 24bit int (3) to move the data pointer.
				data += sizeof(uint24_t);
			}
			return array_16;
		}
	}
}