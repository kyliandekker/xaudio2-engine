#include <uaudio/wave/low_level/WaveConverter.h>

#include <uaudio/utils/uint24_t.h>

namespace uaudio
{
	// Square root of 2 (size of uint16_t)
	constexpr float SQRT_TWO = 1.4142135623730950488016887242097f;

	namespace conversion
	{
		/// <summary>
		/// Recalculates the buffer size from 24-bit to 16-bit.
		/// </summary>
		/// <param name="a_Size">The buffer size.</param>
		/// <returns></returns>
		uint32_t Calculate24To16Size(uint32_t a_Size)
		{
			return a_Size / sizeof(uint24_t) * sizeof(uint16_t);
		}

		/// <summary>
		/// Recalculates the buffer size from 32-bit to 16-bit.
		/// </summary>
		/// <param name="a_Size">The buffer size.</param>
		/// <returns></returns>
		uint32_t Calculate32To16Size(uint32_t a_Size)
		{
			return a_Size / sizeof(uint32_t) * sizeof(uint16_t);
		}

		/// <summary>
		/// Recalculates the buffer size from 24-bit to 32-bit.
		/// </summary>
		/// <param name="a_Size">The buffer size.</param>
		/// <returns></returns>
		uint32_t Calculate24To32Size(uint32_t a_Size)
		{
			return a_Size / sizeof(uint24_t) * sizeof(uint32_t);
		}

		/// <summary>
		/// Recalculates the buffer size from 16-bit to 32-bit.
		/// </summary>
		/// <param name="a_Size">The buffer size.</param>
		/// <returns></returns>
		uint32_t Calculate16To32Size(uint32_t a_Size)
		{
			return a_Size / sizeof(uint16_t) * sizeof(uint32_t);
		}

		/// <summary>
		/// Recalculates the buffer size from 16-bit to 24-bit.
		/// </summary>
		/// <param name="a_Size">The buffer size.</param>
		/// <returns></returns>
		uint32_t Calculate16To24Size(uint32_t a_Size)
		{
			return a_Size / sizeof(uint16_t) * sizeof(uint24_t);
		}

		/// <summary>
		/// Recalculates the buffer size from 32-bit to 24-bit.
		/// </summary>
		/// <param name="a_Size">The buffer size.</param>
		/// <returns></returns>
		uint32_t Calculate32To24Size(uint32_t a_Size)
		{
			return a_Size / sizeof(uint32_t) * sizeof(uint24_t);
		}

		/// <summary>
		/// Recalculates the buffer size from mono to stereo.
		/// </summary>
		/// <param name="a_Size">The buffer size.</param>
		/// <returns></returns>
		uint32_t CalculateMonoToStereoSize(uint32_t a_Size)
		{
			return a_Size * 2;
		}

		/// <summary>
		/// Recalculates the buffer size from stereo to mono.
		/// </summary>
		/// <param name="a_Size">The buffer size.</param>
		/// <returns></returns>
		uint32_t CalculateStereoToMonoSize(uint32_t a_Size)
		{
			return a_Size / 2;
		}

		/// <summary>
		/// Converts 24 bit pcm data to 16 bit pcm data.
		/// </summary>
		/// <param name="a_DataBuffer">The new data buffer.</param>
		/// <param name="a_OriginalDataBuffer">The original data buffer.</param>
		/// <param name="a_Size">The data size (will get changed)</param>
		void Convert24To16(unsigned char *a_DataBuffer, unsigned char *a_OriginalDataBuffer, uint32_t &a_Size)
		{
			// Determine the size of a 16bit data array.
			// Chunk size divided by the size of a 24bit int (3) multiplied by the size of a 16bit int (2).
			const uint32_t newSize = Calculate24To16Size(a_Size);

			uint16_t *array_16 = reinterpret_cast<uint16_t *>(a_DataBuffer);

			int i = 0;
			for (uint32_t a = 0; a < a_Size; a += sizeof(uint24_t), i++)
			{
				// Skip 1 bit every time since we go from 24bit to 16bit.
				array_16[i] = *reinterpret_cast<uint16_t *>(a_OriginalDataBuffer + a + 1);
			}
			a_Size = newSize;
		}

		/// <summary>
		/// Converts 32 bit pcm data to 16 bit pcm data.
		/// </summary>
		/// <param name="a_DataBuffer">The new data buffer.</param>
		/// <param name="a_OriginalDataBuffer">The original data buffer.</param>
		/// <param name="a_Size">The data size (will get changed)</param>
		void Convert32To16(unsigned char *a_DataBuffer, unsigned char *a_OriginalDataBuffer, uint32_t &a_Size)
		{
			// Determine the size of a 16bit data array.
			// Chunk size divided by the size of a 32bit int (4) multiplied by the size of a 16bit int (2).
			uint32_t new_size = Calculate32To16Size(a_Size);

			uint16_t *array_16 = reinterpret_cast<uint16_t *>(a_DataBuffer);

			int i = 0;
			for (uint32_t a = 0; a < a_Size; a += sizeof(uint32_t), i++)
			{
				// Add the size of a 32bit int (4) to move the data pointer.
				float converted_value = *reinterpret_cast<float *>(a_OriginalDataBuffer + a);

				// Calc 32 to 16 bit int.
				converted_value /= SQRT_TWO;
				converted_value *= INT16_MAX;

				array_16[i] = static_cast<uint16_t>(converted_value);
			}
			a_Size = new_size;
		}

		/// <summary>
		/// Converts mono data to stereo data.
		/// </summary>
		/// <param name="a_DataBuffer">The new data buffer.</param>
		/// <param name="a_OriginalDataBuffer">The original data buffer.</param>
		/// <param name="a_Size">The data size (will get changed)</param>
		/// <param name="a_BlockAlign">The alignment of 1 sample.</param>
		void ConvertMonoToStereo(unsigned char *a_DataBuffer, unsigned char *a_OriginalDataBuffer, uint32_t &a_Size, uint16_t a_BlockAlign)
		{
			if (a_Size % a_BlockAlign != 0)
				return;

			// Double the size.
			a_Size = CalculateMonoToStereoSize(a_Size);

			int newIndex = 0;
			for (uint32_t i = 0; i <= a_Size / 2 - a_BlockAlign; i += a_BlockAlign)
			{
				int echo = 0;
				while (echo < 2)
				{
					for (uint32_t j = 0; j < a_BlockAlign; j++)
					{
						a_DataBuffer[newIndex] = a_OriginalDataBuffer[i + j];
						newIndex++;
					}
					echo++;
				}
			}
		}

		/// <summary>
		/// Converts stereo data to mono data.
		/// </summary>
		/// <param name="a_DataBuffer">The new data buffer.</param>
		/// <param name="a_OriginalDataBuffer">The original data buffer.</param>
		/// <param name="a_Size">The data size (will get changed)</param>
		/// <param name="a_BlockAlign">The alignment of 1 sample.</param>
		void ConvertStereoToMono(unsigned char *a_DataBuffer, unsigned char *a_OriginalDataBuffer, uint32_t &a_Size, uint16_t a_BlockAlign)
		{
			if (a_Size % a_BlockAlign != 0)
				return;

			// Double the size.
			a_Size = CalculateStereoToMonoSize(a_Size);

			uint32_t newIndex = 0;
			for (uint32_t i = 0; i <= a_Size * 2 - a_BlockAlign; i += a_BlockAlign)
				for (uint32_t j = 0; j < static_cast<uint32_t>(a_BlockAlign) / 2; j++)
				{
					a_DataBuffer[newIndex] = a_OriginalDataBuffer[i + j];
					newIndex++;
				}
		}

		void ConvertToSamples(float* a_OutSamples, unsigned char* a_DataBuffer, uint32_t a_SampleCount)
		{
			for (uint32_t i = 0; i < a_SampleCount; ++i, a_DataBuffer += 4, ++a_OutSamples)
			{
				short lSample = *(const short*)a_DataBuffer;
				short rSample = *(const short*)(a_DataBuffer + 2);
				*a_OutSamples = ((float)lSample + (float)rSample) / 65536.f;
			}
		}
	}
}