#pragma once

#include <thread>
#include <vector>

#include "xaudio2/XAudio2Channel.h"
#include "Handle.h"
#include "Includes.h"

namespace uaudio
{
	constexpr uint32_t NUM_CHANNELS = 20;

	class AudioSystem
	{
	public:
		AudioSystem();
		virtual ~AudioSystem();

		// Basic default methods for the system.
		void SetPlaybackStatus(bool a_Playback);
		void Stop();
		void Start();
		bool HasPlayback() const;

		IXAudio2 &GetEngine() const;

		// Master effects such as volume and panning.
		void SetMasterVolume(float a_Volume);
		float GetMasterVolume() const;

		void SetMasterPanning(float a_Panning);
		float GetMasterPanning() const;

		uint32_t GetBufferSize() const;

		// Channel-related methods.
		ChannelHandle Play(WaveFile& a_WaveFile);

		uint32_t ChannelSize() const;
		xaudio2::XAudio2Channel* GetChannel(ChannelHandle a_ChannelHandle);

	private:
		void Update();

		IXAudio2 *m_Engine = nullptr;
		IXAudio2MasteringVoice *m_MasterVoice = nullptr;

		BUFFERSIZE m_BufferSize = UAUDIO_DEFAULT_BUFFERSIZE;

		std::vector<xaudio2::XAudio2Channel, UAUDIO_DEFAULT_ALLOCATOR<xaudio2::XAudio2Channel>> m_Channels;

		bool m_Active = true;
		bool m_Playback = true;
		float m_Volume = UAUDIO_DEFAULT_VOLUME;
		float m_Panning = UAUDIO_DEFAULT_PANNING;

		std::thread m_Thread;
	};
}