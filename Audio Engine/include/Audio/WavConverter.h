#pragma once

#include <cstdint>

namespace wav
{
	namespace wav_converter
	{
		uint16_t* convert_24_to_16(unsigned char* data, uint32_t& size);
		uint16_t* convert_32_to_16(unsigned char* data, uint32_t& size);
	}
}