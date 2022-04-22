#pragma once

#include "WaveChunks.h"

enum class LOOP_POINT_SETTING
{
	LOOP_POINT_SETTING_NONE,
	LOOP_POINT_SETTING_START,
	LOOP_POINT_SETTING_END,
	LOOP_POINT_SETTING_BOTH,
};

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

#if !defined(UAUDIO_DEFAULT_SET_LOOP_POINTS)

#define UAUDIO_DEFAULT_SET_LOOP_POINTS LOOP_POINT_SETTING::LOOP_POINT_SETTING_BOTH

#endif

#include <vector>

namespace uaudio
{
	struct Wave_Config
	{
		Wave_Config();
		Wave_Config(const Wave_Config &rhs);

		Wave_Config &operator=(const Wave_Config &rhs);

		std::vector<const char *, UAUDIO_DEFAULT_ALLOCATOR<const char *>> chunksToLoad = {UAUDIO_DEFAULT_CHUNKS};
		uint16_t numChannels = UAUDIO_DEFAULT_CHANNELS;
		uint16_t bitsPerSample = UAUDIO_DEFAULT_BITS_PER_SAMPLE;
		LOOP_POINT_SETTING setLoopPoints = UAUDIO_DEFAULT_SET_LOOP_POINTS;
	};
}
