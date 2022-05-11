#pragma once

#include <uaudio/system/high_level/AudioComponent.h>
#include <uaudio/system/low_level/AudioBackend.h>

#include <uaudio/utils/Utils.h>

namespace uaudio
{
	class AudioSystem
	{
	public:
		AudioSystem();
		~AudioSystem();

		AudioComponent& CreateComponent();

		UAUDIO_RESULT Update();

		UAUDIO_RESULT SetMasterVolume(float a_Volume) const;
		UAUDIO_RESULT GetMasterVolume(float& a_Volume) const;

		UAUDIO_RESULT SetMasterPanning(float a_Panning) const;
		UAUDIO_RESULT GetMasterPanning(float& a_Panning) const;

		UAUDIO_RESULT SetBufferSize(BUFFERSIZE a_BufferSize) const;
		UAUDIO_RESULT GetBufferSize(BUFFERSIZE& a_BufferSize) const;

		UAUDIO_RESULT SetActive(bool a_Active) const;
		UAUDIO_RESULT GetActive(bool& a_Active) const;

		UAUDIO_RESULT SetPlayback(bool a_Playback) const;
		UAUDIO_RESULT GetPlayback(bool& a_Playback) const;

		UAUDIO_RESULT GetComponentsSize(size_t& a_Size) const;
		AudioComponent& GetComponent(uint32_t a_Index);

		UAUDIO_RESULT CreateSoundChannel(int32_t& a_Channel, SoundData& a_SoundData);
		UAUDIO_RESULT CreateSoundChannel(int32_t& a_Channel);

		UAUDIO_RESULT GetSoundChannel(Channel*& a_Channel, int32_t a_Index);
	private:
		utils::uaudio_vector<AudioComponent> m_Components;
		AudioBackend* m_AudioBackend = nullptr;
	};
}
