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

	struct SoundData
	{
		SoundData() = default;
		SoundData(float vol, float pan, bool loop) : volume(vol), panning(pan), looping(loop),
			prev(SOUND_STATE::SOUND_STATE_CREATED), current(prev)  {}

		void SetState(SOUND_STATE soundState) { prev = current; current = soundState; }

		float volume = UAUDIO_DEFAULT_VOLUME;
		float panning = UAUDIO_DEFAULT_PANNING;
		bool looping = false;
		bool changed = false;

		SOUND_STATE prev = SOUND_STATE::SOUND_STATE_INVALID;
		SOUND_STATE current = prev;
	};
}
