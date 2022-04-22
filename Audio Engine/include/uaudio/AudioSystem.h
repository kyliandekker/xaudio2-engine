#pragma once

#include <thread>
#include <vector>

#include <uaudio/xaudio2/XAudio2Channel.h>
#include <uaudio/Handle.h>
#include <uaudio/Includes.h>

enum class AUDIO_MODE
{
	AUDIO_MODE_NORMAL,
	AUDIO_MODE_THREADED,
};

namespace uaudio
{
#if !defined(UAUDIO_DEFAULT_NUM_CHANNELS)

	constexpr uint32_t UAUDIO_DEFAULT_NUM_CHANNELS = 20;

#endif

	class AudioSystem
	{
	public:
		AudioSystem(AUDIO_MODE a_AudioMode = AUDIO_MODE::AUDIO_MODE_THREADED);
		virtual ~AudioSystem();

		// Basic default methods for the system.
		void SetPlaybackStatus(bool a_Playback);
		void Stop();
		void Start();
		bool HasPlayback() const;

		void UpdateNonExtraThread();

		IXAudio2 &GetEngine() const;

		// Master effects such as volume and panning.
		void SetMasterVolume(float a_Volume);
		float GetMasterVolume() const;

		void SetMasterPanning(float a_Panning);
		float GetMasterPanning() const;

		BUFFERSIZE GetBufferSize() const;
		void SetBufferSize(BUFFERSIZE a_BufferSize);

		// Channel-related methods.
		ChannelHandle Play(const WaveFile &a_WaveFile);

		uint32_t ChannelSize() const;
		xaudio2::XAudio2Channel *GetChannel(ChannelHandle a_ChannelHandle);

	private:
		AUDIO_MODE m_AudioMode = AUDIO_MODE::AUDIO_MODE_THREADED;

		void Update();
		void UpdateChannels();

		std::thread m_Thread;

		IXAudio2 *m_Engine = nullptr;
		IXAudio2MasteringVoice *m_MasterVoice = nullptr;

		BUFFERSIZE m_BufferSize = UAUDIO_DEFAULT_BUFFERSIZE;

		std::vector<xaudio2::XAudio2Channel, UAUDIO_DEFAULT_ALLOCATOR<xaudio2::XAudio2Channel>> m_Channels;

		bool m_Active = true;
		bool m_Playback = true;
		float m_Volume = UAUDIO_DEFAULT_VOLUME;
		float m_Panning = UAUDIO_DEFAULT_PANNING;
	};
}