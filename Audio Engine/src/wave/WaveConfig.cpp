#include <utility>

#include "wave/WaveConfig.h"

namespace uaudio
{
	Wave_Config::Wave_Config() = default;

	Wave_Config::Wave_Config(const Wave_Config& rhs) : chunksToLoad(rhs.chunksToLoad), numChannels(rhs.numChannels), bitsPerSample(rhs.bitsPerSample), setLoopPoints(rhs.setLoopPoints)
	{ }

	Wave_Config& Wave_Config::operator=(const Wave_Config& rhs)
	{
		if (&rhs != this)
		{
			chunksToLoad = rhs.chunksToLoad;
			numChannels = rhs.numChannels;
			bitsPerSample = rhs.bitsPerSample;
			setLoopPoints = rhs.setLoopPoints;
		}
		return *this;
	}
}