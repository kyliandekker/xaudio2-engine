#include <cassert>
#include <iostream>

#include <wav/WavConverter.h>

#include <utils/uint24_t.h>

namespace uaudio
{
	// Square root of 2 (size of uint16_t)
	constexpr float SQRT_TWO = 1.4142135623730950488016887242097f;

	namespace conversion
	{
		/// <summary>
		/// Converts 24 bit pcm data to 16 bit pcm data.
		/// </summary>
		/// <param name="a_Data">The actual data.</param>
		/// <param name="a_Size">The data size.</param>
		/// <returns></returns>
		uint16_t* Convert24To16(unsigned char* a_Data, uint32_t& a_Size)
		{
			// Determine the size of a 16bit data array.
			// Chunk size divided by the size of a 32bit int (4) multiplied by the size of a 16bit int (2). 
			a_Size = a_Size / sizeof(uint24_t) * sizeof(uint16_t);

			uint16_t* array_16 = new uint16_t[a_Size];

			for (uint32_t a = 0; a < a_Size; a++)
			{
				// Add the size of a 24bit int (3) to move the data pointer.
				uint24_t converted_value = *reinterpret_cast<uint24_t*>(a_Data + (a * sizeof(uint24_t)) + 1);

				array_16[a] = static_cast<uint16_t>(converted_value);
			}
			return array_16;
		}

		/// <summary>
		/// Converts 32 bit pcm data to 16 bit pcm data.
		/// </summary>
		/// <param name="a_Data">The actual data.</param>
		/// <param name="a_Size">The data size.</param>
		/// <returns></returns>
		uint16_t* Convert32To16(unsigned char* a_Data, uint32_t& a_Size)
		{
			// Determine the size of a 16bit data array.
			// Chunk size divided by the size of a 24bit int (3) multiplied by the size of a 16bit int (2). 
			a_Size = a_Size / sizeof(uint32_t) * sizeof(uint16_t);

			uint16_t* array_16 = new uint16_t[a_Size];

			for (uint32_t a = 0; a < a_Size; a++)
			{
				// Add the size of a 32bit int (4) to move the data pointer.
				float converted_value = *reinterpret_cast<float*>(a_Data + (a * sizeof(uint32_t)));

				//calc 32 to 16 bit int.
				converted_value /= SQRT_TWO;
				converted_value *= INT16_MAX;

				array_16[a] = static_cast<uint16_t>(converted_value);
			}
			return array_16;
		}

		/// <summary>
		/// Converts mono data to stereo data.
		/// </summary>
		/// <param name="a_Data"></param>
		/// <param name="a_Size"></param>
		/// <param name="a_BlockAlign"></param>
		/// <returns></returns>
		unsigned char* ConvertMonoToStereo(unsigned char* a_Data, uint32_t& a_Size, uint16_t a_BlockAlign)
		{
			assert(a_Size % a_BlockAlign == 0);

			// Double the size.
			a_Size = a_Size * 2;

			unsigned char* array_16 = new unsigned char[a_Size];

			int newIndex = 0;

			for (uint16_t i = 0; i <= static_cast<uint16_t>(a_Size / 2) - a_BlockAlign; i += a_BlockAlign)
			{
				int echo = 0;
				while (echo < 2)
				{
					for (uint16_t j = 0; j < a_BlockAlign; j++)
					{
						array_16[newIndex] = static_cast<uint16_t>(a_Data[i + j]);
						newIndex++;
					}
					echo++;
				}
			}

			return array_16;
		}

		/// <summary>
		/// Converts stereo data to mono data.
		/// </summary>
		/// <param name="a_Data"></param>
		/// <param name="a_Size"></param>
		/// <param name="a_BlockAlign"></param>
		/// <returns></returns>
		unsigned char* ConvertStereoToMono(unsigned char* a_Data, uint32_t& a_Size, uint16_t a_BlockAlign)
		{
			assert(a_Size % a_BlockAlign == 0);

			// Double the size.
			a_Size = a_Size / 2;

			unsigned char* array_16 = new unsigned char[a_Size];

			int newIndex = 0;
			for (uint16_t i = 0; i <= static_cast<uint16_t>(a_Size * 2) - a_BlockAlign; i += a_BlockAlign)
			{
				for (uint16_t j = 0; j < a_BlockAlign / 2; j++)
				{
					array_16[newIndex] = static_cast<uint16_t>(a_Data[i + j]);
					newIndex++;
				}
			}

			return array_16;
		}
	}
}