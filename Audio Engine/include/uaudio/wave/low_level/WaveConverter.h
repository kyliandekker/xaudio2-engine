#pragma once

#include <cstdint>

namespace uaudio
{
	namespace conversion
	{
		uint32_t Calculate24To16Size(uint32_t a_Size);
		uint32_t Calculate32To16Size(uint32_t a_Size);
		uint32_t CalculateMonoToStereoSize(uint32_t a_Size);
		uint32_t CalculateStereoToMonoSize(uint32_t a_Size);
		void Convert24To16(unsigned char* a_Data, unsigned char* a_Original_Data, uint32_t& a_Size);
		void Convert32To16(unsigned char* a_Data, unsigned char* a_Original_Data, uint32_t& a_Size);
		void ConvertMonoToStereo(unsigned char* a_Data, unsigned char* a_Original_Data, uint32_t& a_Size, uint16_t a_BlockAlign);
		void ConvertStereoToMono(unsigned char* a_Data, unsigned char* a_Original_Data, uint32_t& a_Size, uint16_t a_BlockAlign);
	}
}