#pragma once

#include <cstdint>

namespace uaudio
{
	namespace effects
	{
		void ChangeVolume(unsigned char*& a_Data, uint32_t a_Size, float a_Volume);
		void ChangePanning(unsigned char*& a_Data, uint32_t a_Size, float a_Amount, uint16_t a_NumChannels);
	}
}