#pragma once

#include <cstdint>

#include <uaudio/generic/UaudioResult.h>

namespace uaudio
{
	namespace conversion
	{
		uint32_t Calculate24To16Size(uint32_t a_Size);
		uint32_t Calculate32To16Size(uint32_t a_Size);

		uint32_t Calculate24To32Size(uint32_t a_Size);
		uint32_t Calculate16To32Size(uint32_t a_Size);

		uint32_t Calculate16To24Size(uint32_t a_Size);
		uint32_t Calculate32To24Size(uint32_t a_Size);

		uint32_t CalculateMonoToStereoSize(uint32_t a_Size);
		uint32_t CalculateStereoToMonoSize(uint32_t a_Size);

		UAUDIO_RESULT Convert24To16(unsigned char *a_DataBuffer, unsigned char *a_OriginalDataBuffer, uint32_t &a_Size);
		UAUDIO_RESULT Convert32To16(unsigned char *a_DataBuffer, unsigned char *a_OriginalDataBuffer, uint32_t &a_Size);
		UAUDIO_RESULT ConvertMonoToStereo(unsigned char *a_DataBuffer, unsigned char *a_OriginalDataBuffer, uint32_t &a_Size, uint16_t a_BlockAlign);
		UAUDIO_RESULT ConvertStereoToMono(unsigned char *a_DataBuffer, unsigned char *a_OriginalDataBuffer, uint32_t &a_Size, uint16_t a_BlockAlign);

		UAUDIO_RESULT ConvertToSamples(float* a_OutSamples, unsigned char* a_DataBuffer, uint32_t a_SampleCount);
	}
}
