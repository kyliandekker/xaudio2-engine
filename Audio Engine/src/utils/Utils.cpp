#include <uaudio/utils/Utils.h>

#include "uaudio/Defines.h"

namespace uaudio::utils
{
	/// <summary>
	/// Converts byte position to milliseconds.
	/// </summary>
	/// <param name="m_Pos">Position in bytes.</param>
	/// <param name="a_ByteRate">The byte rate.</param>
	/// <returns>Returns the value in milliseconds.</returns>
	float PosToMilliseconds(uint32_t m_Pos, uint32_t a_ByteRate)
	{
		return SecondsToMilliseconds(PosToSeconds(m_Pos, a_ByteRate));
	}

	/// <summary>
	/// Converts byte position to seconds.
	/// </summary>
	/// <param name="m_Pos">Position in bytes.</param>
	/// <param name="a_ByteRate">The byte rate.</param>
	/// <returns>Returns the value in seconds.</returns>
	float PosToSeconds(uint32_t m_Pos, uint32_t a_ByteRate)
	{
		if (a_ByteRate == 0)
			return 0;

		return static_cast<float>(m_Pos) / static_cast<float>(a_ByteRate);
	}

	/// <summary>
	/// Converts seconds to byte position.
	/// </summary>
	/// <param name="m_Seconds">Seconds.</param>
	/// <param name="a_ByteRate">The byte rate.</param>
	/// <returns>Returns the value in byte position.</returns>
	uint32_t SecondsToPos(float m_Seconds, uint32_t a_ByteRate)
	{
		if (a_ByteRate == 0)
			return 0;

		return static_cast<uint32_t>(m_Seconds * static_cast<float>(a_ByteRate));
	}

	/// <summary>
	/// Converts milliseconds to byte position.
	/// </summary>
	/// <param name="m_Milliseconds">Milliseconds.</param>
	/// <param name="a_ByteRate">The byte rate.</param>
	/// <returns>Returns the value in byte position.</returns>
	uint32_t MillisecondsToPos(float m_Milliseconds, uint32_t a_ByteRate)
	{
		if (a_ByteRate == 0)
			return 0;

		return SecondsToPos(MillisecondsToSeconds(m_Milliseconds), a_ByteRate);
	}

	/// <summary>
	/// Converts seconds to milliseconds.
	/// </summary>
	/// <param name="m_Seconds">Seconds.</param>
	/// <returns>Returns the value in milliseconds.</returns>
	float SecondsToMilliseconds(float m_Seconds)
	{
		return m_Seconds * 1000;
	}

	/// <summary>
	/// Converts milliseconds to seconds.
	/// </summary>
	/// <param name="m_Milliseconds">Milliseconds</param>
	/// <returns>Returns the value in seconds.</returns>
	float MillisecondsToSeconds(float m_Milliseconds)
	{
		return m_Milliseconds / 1000;
	}

	/// <summary>
	/// Converts seconds to hours.
	/// </summary>
	/// <param name="m_Seconds">Seconds.</param>
	/// <returns>Returns the value in hours.</returns>
	uint32_t SecondsToHours(float m_Seconds)
	{
		return static_cast<uint32_t>(m_Seconds) / 3600;
	}

	/// <summary>
	/// Converts seconds to minutes.
	/// </summary>
	/// <param name="m_Seconds">Seconds.</param>
	/// <returns>Returns the value in minutes.</returns>
	uint32_t SecondsToMinutes(float m_Seconds)
	{
		return (static_cast<uint32_t>(m_Seconds) - (SecondsToHours(m_Seconds) * 3600)) / 60;
	}

	/// <summary>
	/// Calculates the duration in seconds from chunk size and byte rate.
	/// </summary>
	/// <param name="a_ChunkSize">The size of the data buffer.</param>
	/// <param name="a_ByteRate">The byte rate.</param>
	/// <returns>Returns the duration in seconds.</returns>
	float GetDuration(uint32_t a_ChunkSize, uint32_t a_ByteRate)
	{
		return static_cast<float>(a_ChunkSize) / static_cast<float>(a_ByteRate);
	}

	/// <summary>
	/// Formats a string with the hours, minutes, seconds and (optional) milliseconds.
	/// </summary>
	/// <param name="a_Duration">Duration in seconds.</param>
	/// <param name="a_Milliseconds">Whether or not milliseconds should be shown.</param>
	/// <returns>Returns the duration in minute:seconds.</returns>
	std::string FormatDuration(float a_Duration, bool a_Milliseconds)
	{
		const uint32_t hours = utils::SecondsToHours(a_Duration);
		const uint32_t minutes = utils::SecondsToMinutes(a_Duration);
		const uint32_t seconds = static_cast<uint32_t>(a_Duration) % 60;
		const uint32_t total = (hours * 3600) + (minutes * 60) + seconds;
		const float milliseconds_float = a_Duration - static_cast<float>(total);
		const uint32_t milliseconds = static_cast<uint32_t>(milliseconds_float * 1000);

		char hours_string[32], minutes_string[32], seconds_string[32], milliseconds_string[32];
		sprintf_s(hours_string, "%02d", hours);
		sprintf_s(minutes_string, "%02d", minutes);
		sprintf_s(seconds_string, "%02d", seconds);
		sprintf_s(milliseconds_string, "%03d", milliseconds);
		return std::string(hours_string) +
			":" +
			std::string(minutes_string) +
			":" +
			std::string(seconds_string) + (a_Milliseconds ? ":" + std::string(milliseconds_string) : "");
	}

	bool chunkcmp(const char* a_ChunkID1, const char* a_ChunkID2)
	{
		return (strncmp(a_ChunkID1, a_ChunkID2, uaudio::CHUNK_ID_SIZE) == 0);
	}
}
