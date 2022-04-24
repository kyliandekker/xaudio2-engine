#pragma once

#include <uaudio/Includes.h>
#include <uaudio/Defines.h>

#include <vector>

namespace uaudio
{
#if !defined(UAUDIO_DEFAULT_CHUNKS)

	#define UAUDIO_DEFAULT_CHUNKS DATA_CHUNK_ID, FMT_CHUNK_ID

#endif

#if !defined(UAUDIO_DEFAULT_CHANNELS)

	#define UAUDIO_DEFAULT_CHANNELS 0

#endif

#if !defined(UAUDIO_DEFAULT_BITS_PER_SAMPLE)

	#define UAUDIO_DEFAULT_BITS_PER_SAMPLE WAVE_BITS_PER_SAMPLE_16

#endif

#if !defined(UAUDIO_DEFAULT_SET_LOOP_POINTS)

	#define UAUDIO_DEFAULT_SET_LOOP_POINTS LOOP_POINT_SETTING::LOOP_POINT_SETTING_BOTH

#endif

	/*
	 * WHAT IS THIS FILE?
	 * This wave config is used in the wave reader. It contains settings related to the wave loading.
	 * Currently there are these settings:
		* Which chunks to load (in a vector of const char*)
		* How many channels the file should have (stereo or mono)
		* How many bits per sample the file should have (16-bit, 24-bit, 32-bit)
		* If the file needs to load loop points and set them automatically.
	 * Conversion will take place if a file does not have these settings present.
	 */
	struct WaveConfig
	{
		WaveConfig();
		WaveConfig(const WaveConfig &rhs);

		WaveConfig &operator=(const WaveConfig &rhs);

		std::vector<const char *, UAUDIO_DEFAULT_ALLOCATOR<const char *>> chunksToLoad = {UAUDIO_DEFAULT_CHUNKS};
		uint16_t numChannels = UAUDIO_DEFAULT_CHANNELS;
		uint16_t bitsPerSample = UAUDIO_DEFAULT_BITS_PER_SAMPLE;
		LOOP_POINT_SETTING setLoopPoints = UAUDIO_DEFAULT_SET_LOOP_POINTS;
	};
}
