#pragma once

#include <cstdint>

namespace effects
{
	unsigned char* ChangeVolume(unsigned char* a_Data, uint32_t a_Size, float a_Volume);
	unsigned char* ChangePanning(unsigned char* a_Data, uint32_t a_Size, float a_Amount);
}