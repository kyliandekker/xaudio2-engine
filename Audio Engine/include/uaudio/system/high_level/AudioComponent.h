#pragma once

#include <uaudio/Includes.h>

#include <uaudio/system/low_level/SoundData.h>

#include <uaudio/system/high_level/UaudioResult.h>

namespace uaudio
{
	class AudioSystem;

	class AudioComponent
	{
	public:
		AudioComponent() = default;
		AudioComponent(const AudioComponent& rhs);

		AudioComponent& operator=(const AudioComponent& rhs);

		UAUDIO_RESULT Play(UAUDIO_DEFAULT_HASH a_Hash);

		UAUDIO_RESULT Pause();
		UAUDIO_RESULT Resume();
		UAUDIO_RESULT IsPlaying(bool& a_IsPlaying) const;

		UAUDIO_RESULT SetVolume(float a_Volume);
		UAUDIO_RESULT GetVolume(float& a_Volume) const;

		UAUDIO_RESULT SetPanning(float a_Panning);
		UAUDIO_RESULT GetPanning(float& a_Panning) const;

		UAUDIO_RESULT SetLooping(bool a_IsLooping);
		UAUDIO_RESULT IsLooping(bool& a_IsLooping) const;

		UAUDIO_RESULT SetPlayback(bool a_Playback);
		UAUDIO_RESULT GetPlayback(bool& a_Playback) const;

		UAUDIO_RESULT IsActive(bool& a_IsActive) const;

		UAUDIO_RESULT GetHash(UAUDIO_DEFAULT_HASH& a_Hash) const;
		UAUDIO_RESULT GetChannelIndex(int32_t& a_Index) const;
	private:
		SoundData m_SoundData;

		friend AudioSystem;
	};
}
