#pragma once

#include <cstdint>

namespace uaudio
{
	namespace effects
	{
		template <class T>
		T clamp(T value, T min, T max)
		{
			if (value < min)
				value = min;
			if (value > max)
				value = max;
			return value;
		}

		/// <summary>
		/// Changes the value of a byte.
		/// </summary>
		/// <param name="a_Value">The byte.</param>
		/// <param name="a_Volume">The volume.</param>
		/// <returns></returns>
		template <class T>
		inline T ChangeByteVolume(T a_Value, float a_Volume)
		{
			float converted_value = static_cast<float>(a_Value);
			converted_value *= a_Volume;

			return static_cast<T>(converted_value);
		}

		/// <summary>
		/// Changes the volume of pcm data.
		/// </summary>
		/// <param name="a_Data">The actual data.</param>
		/// <param name="a_Size">The data size.</param>
		/// <param name="a_Volume">The volume.</param>
		/// <returns></returns>
		template <class T>
		inline void ChangeVolume(unsigned char *&a_Data, uint32_t a_Size, float a_Volume, uint16_t, uint16_t)
		{
			// Clamp the volume to 0.0 min and 1.0 max.
			a_Volume = clamp(a_Volume, 0.0f, 1.0f);

			T *array_16 = reinterpret_cast<T *>(a_Data);
			for (uint32_t i = 0; i < (a_Size / sizeof(T)); i++)
				array_16[i] = ChangeByteVolume<T>(array_16[i], a_Volume);

			a_Data = reinterpret_cast<unsigned char *>(array_16);
		}

		/// <summary>
		/// Changes the volume of pcm data.
		/// </summary>
		/// <param name="a_Data">The actual data.</param>
		/// <param name="a_Size">The data size.</param>
		/// <param name="a_Amount">The panning amount (-1 is fully left, 1 is fully right, 0 is middle).</param>
		/// <returns></returns>
		template <class T>
		inline void ChangePanning(unsigned char *&a_Data, uint32_t a_Size, float a_Amount, uint16_t a_NumChannels)
		{
			if (a_NumChannels == 1)
				return;

			// Amount is a value from -1 to 1.
			a_Amount = clamp(a_Amount, -1.0f, 1.0f);

			// Set the values to 1.0 as default.
			float left = 1.0f, right = 1.0f;

			// If the slider is more to the left.
			if (a_Amount < 0)
			{
				right += a_Amount;
				// Clamp the volume to 0.0 min and 1.0 max.
				right = clamp(right, 0.0f, 1.0f);
			}
			// If the slider is more to the right.
			else if (a_Amount > 0)
			{
				left -= a_Amount;
				// Clamp the volume to 0.0 min and 1.0 max.
				left = clamp(left, 0.0f, 1.0f);
			}

			T *array_16 = reinterpret_cast<T *>(a_Data);
			for (uint32_t i = 0; i < (a_Size / sizeof(T)); i += a_NumChannels)
			{
				array_16[i] = ChangeByteVolume<T>(array_16[i], left);
				array_16[i + 1] = ChangeByteVolume<T>(array_16[i + 1], right);
			}

			a_Data = reinterpret_cast<unsigned char *>(array_16);
		}
	}
}