#pragma once

#include <cstdint>

namespace uaudio
{
	namespace conversion
	{
		uint16_t* Convert24To16(unsigned char* a_Data, uint32_t& a_Size);
		uint16_t* Convert32To16(unsigned char* a_Data, uint32_t& a_Size);
		unsigned char* ConvertMonoToStereo(unsigned char* a_Data, uint32_t& a_Size, uint16_t a_BlockAlign);
		unsigned char* ConvertStereoToMono(unsigned char* a_Data, uint32_t& a_Size, uint16_t a_BlockAlign);
	}
}