#pragma once

#include "WaveChunks.h"

#include "UserInclude.h"

#if !defined(UAUDIO_DEFAULT_CHUNKS)

#define UAUDIO_DEFAULT_CHUNKS DATA_CHUNK_ID, FMT_CHUNK_ID

#endif

#if !defined(UAUDIO_DEFAULT_CHANNELS)

#define UAUDIO_DEFAULT_CHANNELS WAVE_CHANNELS_STEREO

#endif

#if !defined(UAUDIO_DEFAULT_BITS_PER_SAMPLE)

#define UAUDIO_DEFAULT_BITS_PER_SAMPLE WAVE_BITS_PER_SAMPLE_16

#endif

#include <vector>

namespace uaudio
{
	struct Wave_Config
	{
		Wave_Config();
		Wave_Config(std::vector<const char *> a_ChunksToLoad, uint16_t a_NumChannels, uint16_t a_BitsPerSample);
		Wave_Config(const Wave_Config &rhs);

		Wave_Config &operator=(const Wave_Config &rhs);

		std::vector<const char *> chunksToLoad = {UAUDIO_DEFAULT_CHUNKS};
		uint16_t numChannels = UAUDIO_DEFAULT_CHANNELS;
		uint16_t bitsPerSample = UAUDIO_DEFAULT_BITS_PER_SAMPLE;
	};
}
