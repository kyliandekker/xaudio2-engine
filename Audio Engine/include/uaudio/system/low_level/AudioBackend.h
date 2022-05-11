#pragma once

#include <uaudio/system/high_level/AudioComponent.h>

namespace uaudio
{
	class AudioSystem;

	class AudioBackend
	{
	public:
		AudioBackend() = default;
		virtual ~AudioBackend() = default;

		void SetMasterVolume(float a_Volume) { m_Volume = a_Volume; }
		float GetMasterVolume() const { return m_Volume; }

		void SetMasterPanning(float a_Panning) { m_Panning = a_Panning; }
		float GetMasterPanning() const { return m_Panning; }

		void SetActive(bool a_Active) { m_Active = a_Active; };
		bool GetActive() const { return m_Active; };

		void SetPlayback(bool a_Playback) { m_Playback = a_Playback; }
		bool GetPlayback() const { return m_Playback; }

		virtual int32_t CreateSoundChannel(SoundData &a_SoundData) = 0;
		virtual int32_t CreateSoundChannel() = 0;

		BUFFERSIZE GetBufferSize() const { return m_BufferSize; }
		void SetBufferSize(BUFFERSIZE a_BufferSize) { m_BufferSize = a_BufferSize; }

		virtual void Update() = 0;

	protected:
		virtual Channel* GetChannel(int32_t a_Index) = 0;

		float m_Volume = UAUDIO_DEFAULT_VOLUME;
		float m_Panning = UAUDIO_DEFAULT_PANNING;
		BUFFERSIZE m_BufferSize = UAUDIO_DEFAULT_BUFFERSIZE;

		bool m_Active = true;
		bool m_Playback = true;

		friend AudioSystem;
	};
}
