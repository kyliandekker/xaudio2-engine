#pragma once

#include <uaudio/Includes.h>
#include <vector>
#include <cstdint>
#include <string>

namespace uaudio::utils
{
	template <class T>
	struct uaudio_vector : std::vector<T, UAUDIO_DEFAULT_ALLOCATOR<T>>
	{};

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
	/// Adds a specified size to a pointer.
	/// </summary>
	/// <param name="a_Ptr">The pointer</param>
	/// <param name="a_Size">The size that needs to be added.</param>
	/// <returns></returns>
	inline void* add(void* a_Ptr, size_t a_Size)
	{
		return reinterpret_cast<unsigned char*>(a_Ptr) + a_Size;
	}

	float PosToMilliseconds(uint32_t m_Pos, uint32_t a_ByteRate);
	float PosToSeconds(uint32_t m_Pos, uint32_t a_ByteRate);
	uint32_t SecondsToPos(float m_Seconds, uint32_t a_ByteRate);
	uint32_t MillisecondsToPos(float m_Milliseconds, uint32_t a_ByteRate);
	float SecondsToMilliseconds(float m_Seconds);
	float MillisecondsToSeconds(float m_Milliseconds);
	uint32_t SecondsToHours(float m_Seconds);
	uint32_t SecondsToMinutes(float m_Seconds);
	float GetDuration(uint32_t a_ChunkSize, uint32_t a_ByteRate);

	std::string FormatDuration(float a_Duration, bool a_Milliseconds = true);

	bool chunkcmp(const char* a_ChunkID1, const char* a_ChunkID2);
}
