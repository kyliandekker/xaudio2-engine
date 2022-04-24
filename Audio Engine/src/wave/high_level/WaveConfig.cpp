#include <utility>

#include <uaudio/wave/high_level/WaveConfig.h>

namespace uaudio
{
	WaveConfig::WaveConfig() = default;

	WaveConfig::WaveConfig(const WaveConfig& rhs) : chunksToLoad(rhs.chunksToLoad), numChannels(rhs.numChannels), bitsPerSample(rhs.bitsPerSample), setLoopPoints(rhs.setLoopPoints)
	{ }

	WaveConfig& WaveConfig::operator=(const WaveConfig& rhs)
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