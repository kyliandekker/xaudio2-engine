#pragma once

#include <cstdint>

namespace wav
{
	namespace wav_converter
	{
		uint16_t* Convert24To16(unsigned char* a_Data, uint32_t& a_Size);
		uint16_t* Convert32To16(unsigned char* a_Data, uint32_t& a_Size);
	}
}