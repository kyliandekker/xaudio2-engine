#include "utils/Utils.h"

namespace uaudio::utils
{
	/// <summary>
	/// Converts byte position to milliseconds.
	/// </summary>
	/// <param name="m_Pos">Position in bytes.</param>
	/// <param name="a_ByteRate">The byte rate.</param>
	/// <returns></returns>
	float PosToMilliseconds(uint32_t m_Pos, uint32_t a_ByteRate)
	{
		return SecondsToMilliseconds(PosToSeconds(m_Pos, a_ByteRate));
	}

	/// <summary>
	/// Converts byte position to seconds.
	/// </summary>
	/// <param name="m_Pos">Position in bytes.</param>
	/// <param name="a_ByteRate">The byte rate.</param>
	/// <returns></returns>
	float PosToSeconds(uint32_t m_Pos, uint32_t a_ByteRate)
	{
		return static_cast<float>(m_Pos) / static_cast<float>(a_ByteRate);
	}

	/// <summary>
	/// Converts seconds to byte position.
	/// </summary>
	/// <param name="m_Seconds">Seconds.</param>
	/// <param name="a_ByteRate">The byte rate.</param>
	/// <returns></returns>
	uint32_t SecondsToPos(float m_Seconds, uint32_t a_ByteRate)
	{
		return static_cast<uint32_t>(m_Seconds * static_cast<float>(a_ByteRate));
	}

	/// <summary>
	/// Converts milliseconds to byte position.
	/// </summary>
	/// <param name="m_Milliseconds">Milliseconds.</param>
	/// <param name="a_ByteRate">The byte rate.</param>
	/// <returns></returns>
	uint32_t MillisecondsToPos(float m_Milliseconds, uint32_t a_ByteRate)
	{
		return SecondsToPos(MillisecondsToSeconds(m_Milliseconds), a_ByteRate);
	}

	/// <summary>
	/// Converts seconds to milliseconds.
	/// </summary>
	/// <param name="m_Seconds">Seconds.</param>
	/// <returns></returns>
	float SecondsToMilliseconds(float m_Seconds)
	{
		return m_Seconds * 1000;
	}

	/// <summary>
	/// Converts milliseconds to seconds.
	/// </summary>
	/// <param name="m_Milliseconds">Milliseconds</param>
	/// <returns></returns>
	float MillisecondsToSeconds(float m_Milliseconds)
	{
		return m_Milliseconds / 1000;
	}

	/// <summary>
	/// Converts seconds to hours.
	/// </summary>
	/// <param name="m_Seconds">Seconds.</param>
	/// <returns></returns>
	uint32_t SecondsToHours(float m_Seconds)
	{
		return static_cast<uint32_t>(m_Seconds) / 3600;
	}

	/// <summary>
	/// Converts seconds to minutes.
	/// </summary>
	/// <param name="m_Seconds">Seconds.</param>
	/// <returns></returns>
	uint32_t SecondsToMinutes(float m_Seconds)
	{
		return (static_cast<uint32_t>(m_Seconds) - (SecondsToHours(m_Seconds) * 3600)) / 60;
	}
}