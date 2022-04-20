#pragma once

#include <cstdint>

namespace uaudio
{
	namespace conversion
	{
		float PosToMilliseconds(uint32_t m_Pos, uint32_t a_ByteRate);
		float PosToSeconds(uint32_t m_Pos, uint32_t a_ByteRate);
		uint32_t SecondsToPos(float m_Seconds, uint32_t a_ByteRate);
		uint32_t MillisecondsToPos(float m_Milliseconds, uint32_t a_ByteRate);
		float SecondsToMilliseconds(float m_Seconds);
		float MillisecondsToSeconds(float m_Milliseconds);
		uint32_t SecondsToHours(float m_Seconds);
		uint32_t SecondsToMinutes(float m_Seconds);

		uint16_t* Convert24To16(unsigned char* a_Data, uint32_t& a_Size);
		uint16_t* Convert32To16(unsigned char* a_Data, uint32_t& a_Size);
		unsigned char* ConvertMonoToStereo(unsigned char* a_Data, uint32_t& a_Size, uint16_t a_BlockAlign);
		unsigned char* ConvertStereoToMono(unsigned char* a_Data, uint32_t& a_Size, uint16_t a_BlockAlign);
	}
}