#include <iostream>

#include "Audio/WavConverter.h"

#include "Audio/uint24_t.h"

namespace wav
{
	// Square root of 2 (size of uint16_t)
	constexpr float sqrt_two = 1.4142135623730950488016887242097f;

	namespace wav_converter
	{
		uint16_t* convert_24_to_16(unsigned char* data, uint32_t& size)
		{
			// Determine the size of a 16bit data array.
			// Chunksize divided by the size of a 32bit int (4) multiplied by the size of a 16bit int (2). 
			size = size / sizeof(uint32_t) * sizeof(uint16_t);

			uint16_t* array_16 = new uint16_t[size];

			for (uint32_t a = 0; a < size; a++)
			{
				float convert_value = *reinterpret_cast<float*>(data);
				
				array_16[a] = static_cast<uint16_t>(convert_value);

				// Add the size of a 24bit int (3) to move the data pointer.
				data += sizeof(uint24_t);
			}
			return array_16;
		}

		uint16_t* convert_32_to_16(unsigned char* data, uint32_t& size)
		{
			// Determine the size of a 16bit data array.
			// Chunksize divided by the size of a 24bit int (3) multiplied by the size of a 16bit int (2). 
			size = size/ sizeof(uint24_t) * sizeof(uint16_t);

			uint16_t* array_16 = new uint16_t[size];

			for (uint32_t a = 0; a < size; a++)
			{
				float convert_value = *reinterpret_cast<float*>(data);

				//calc 32 to 16 bit unsigned int.
				convert_value /= sqrt_two;
				convert_value *= INT16_MAX;

				array_16[a] = static_cast<uint16_t>(convert_value);

				// Add the size of a 32bit int (4) to move the data pointer.
				data += sizeof(uint32_t);
			}
			return array_16;
		}
	}
}
