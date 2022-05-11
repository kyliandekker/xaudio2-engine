#include <uaudio/system/low_level/SoundData.h>

namespace uaudio
{
	SoundData::SoundData(const SoundData& rhs) : soundHash(rhs.soundHash), volume(rhs.volume), panning(rhs.panning), looping(rhs.looping), changed(rhs.changed), channel(rhs.channel), prev(rhs.prev), current(rhs.current)
	{ }

	SoundData& SoundData::operator=(const SoundData& rhs)
	{
		if (&rhs != this)
		{
			soundHash = rhs.soundHash;
			volume = rhs.volume;
			panning = rhs.panning;
			looping = rhs.looping;
			changed = rhs.changed;
			channel = rhs.channel;
			prev = rhs.prev;
			current = rhs.current;
		}
		return *this;
	}
}