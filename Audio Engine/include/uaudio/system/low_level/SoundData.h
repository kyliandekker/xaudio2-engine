#pragma once

#include <uaudio/Includes.h>

namespace uaudio
{
	enum class SOUND_STATE
	{
		SOUND_STATE_INVALID,
		SOUND_STATE_CREATED,
		SOUND_STATE_PLAYING,
		SOUND_STATE_PAUSED,
		SOUND_STATE_FINISHED
	};

	class Channel;
	class AudioComponent;
	class AudioSystem;

	struct SoundData
	{
		SoundData() { current = SOUND_STATE::SOUND_STATE_CREATED; }
		SoundData(UAUDIO_DEFAULT_HASH hash, float vol, float pan, bool loop) : soundHash(hash), volume(vol), panning(pan), looping(loop), prev(SOUND_STATE::SOUND_STATE_CREATED), current(prev)  {}
		SoundData(const SoundData& rhs);

		SoundData& operator=(const SoundData& rhs);

		void SetState(SOUND_STATE soundState) { prev = current; current = soundState; }
		bool HasChanged() const { return changed; }

		UAUDIO_DEFAULT_HASH soundHash = 0;
		float volume = UAUDIO_DEFAULT_VOLUME;
		float panning = UAUDIO_DEFAULT_PANNING;
		bool looping = false;
		bool changed = false;
		bool playback = true;
		int32_t channel = -1;

		SOUND_STATE prev = SOUND_STATE::SOUND_STATE_INVALID;
		SOUND_STATE current = prev;
	};
}
