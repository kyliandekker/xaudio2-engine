#pragma once

#include <cstdint>

namespace uaudio::utils
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

	inline void* add(void* a_Ptr, size_t a_Size);

	float PosToMilliseconds(uint32_t m_Pos, uint32_t a_ByteRate);
	float PosToSeconds(uint32_t m_Pos, uint32_t a_ByteRate);
	uint32_t SecondsToPos(float m_Seconds, uint32_t a_ByteRate);
	uint32_t MillisecondsToPos(float m_Milliseconds, uint32_t a_ByteRate);
	float SecondsToMilliseconds(float m_Seconds);
	float MillisecondsToSeconds(float m_Milliseconds);
	uint32_t SecondsToHours(float m_Seconds);
	uint32_t SecondsToMinutes(float m_Seconds);
	float GetDuration(uint32_t a_ChunkSize, uint32_t a_ByteRate);
}