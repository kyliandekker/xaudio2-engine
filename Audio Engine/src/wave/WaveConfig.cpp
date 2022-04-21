#include <utility>

#include "wave/WaveConfig.h"

namespace uaudio
{
	Wave_Config::Wave_Config() = default;

	Wave_Config::Wave_Config(std::vector<const char*> a_ChunksToLoad, uint16_t a_NumChannels, uint16_t a_BitsPerSample) : chunksToLoad(
		std::move(a_ChunksToLoad)), numChannels(a_NumChannels), bitsPerSample(a_BitsPerSample)
	{ }

	Wave_Config::Wave_Config(const Wave_Config& rhs) : chunksToLoad(rhs.chunksToLoad), numChannels(rhs.numChannels), bitsPerSample(rhs.bitsPerSample)
	{ }

	Wave_Config& Wave_Config::operator=(const Wave_Config& rhs)
	{
		if (&rhs != this)
		{
			chunksToLoad = rhs.chunksToLoad;
			numChannels = rhs.numChannels;
			bitsPerSample = rhs.bitsPerSample;
		}
		return *this;
	}
}