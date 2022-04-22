#pragma once

#include <cstdint>

#include <uaudio/utils/Utils.h>

namespace uaudio
{
	namespace effects
	{
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
		/// <param name="a_DataBuffer">The actual data.</param>
		/// <param name="a_Size">The data size.</param>
		/// <param name="a_Volume">The volume.</param>
		/// <returns></returns>
		template <class T>
		inline void ChangeVolume(unsigned char *&a_DataBuffer, uint32_t a_Size, float a_Volume, uint16_t, uint16_t)
		{
			// Clamp the volume to 0.0 min and 1.0 max.
			a_Volume = utils::clamp(a_Volume, UAUDIO_MIN_VOLUME, UAUDIO_MAX_VOLUME);

			T *array_16 = reinterpret_cast<T *>(a_DataBuffer);
			for (uint32_t i = 0; i < (a_Size / sizeof(T)); i++)
				array_16[i] = ChangeByteVolume<T>(array_16[i], a_Volume);

			a_DataBuffer = reinterpret_cast<unsigned char *>(array_16);
		}

		/// <summary>
		/// Changes the volume of pcm data.
		/// </summary>
		/// <param name="a_DataBuffer">The actual data.</param>
		/// <param name="a_Size">The data size.</param>
		/// <param name="a_Amount">The panning amount (-1 is fully left, 1 is fully right, 0 is middle).</param>
		/// <param name="a_NumChannels">The number of channels (mono or stereo).</param>
		/// <returns></returns>
		template <class T>
		inline void ChangePanning(unsigned char *&a_DataBuffer, uint32_t a_Size, float a_Amount, uint16_t a_NumChannels)
		{
			if (a_NumChannels == 1)
				return;

			// Amount is a value from -1 to 1.
			a_Amount = utils::clamp(a_Amount, UAUDIO_MIN_PANNING, UAUDIO_MAX_PANNING);

			// Set the values to 1.0 as default.
			float left = UAUDIO_MAX_VOLUME, right = UAUDIO_MAX_VOLUME;

			// If the slider is more to the left.
			if (a_Amount < 0)
			{
				right += a_Amount;
				// Clamp the volume to 0.0 min and 1.0 max.
				right = utils::clamp(right, UAUDIO_MIN_VOLUME, UAUDIO_MAX_VOLUME);
			}
			// If the slider is more to the right.
			else if (a_Amount > 0)
			{
				left -= a_Amount;
				// Clamp the volume to 0.0 min and 1.0 max.
				left = utils::clamp(left, UAUDIO_MIN_VOLUME, UAUDIO_MAX_VOLUME);
			}

			T *array_16 = reinterpret_cast<T *>(a_DataBuffer);
			for (uint32_t i = 0; i < (a_Size / sizeof(T)); i += a_NumChannels)
			{
				array_16[i] = ChangeByteVolume<T>(array_16[i], left);
				array_16[i + 1] = ChangeByteVolume<T>(array_16[i + 1], right);
			}

			a_DataBuffer = reinterpret_cast<unsigned char *>(array_16);
		}
	}
}
