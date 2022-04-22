#include <uaudio/AudioSystem.h>

#include <uaudio/utils/Logger.h>

#include <xaudio2.h>

#include <uaudio/SoundSystem.h>
#include <uaudio/wave/low_level/WaveEffects.h>

namespace uaudio
{
	AudioSystem::AudioSystem(AUDIO_MODE a_AudioMode) : m_AudioMode(a_AudioMode)
	{
		HRESULT hr;
		if (FAILED(hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
		{
			logger::ASSERT(false, "Initializing COM library failed.");
			logger::log_error("<XAudio2> Initializing COM library failed.");
			return;
		}

		m_Engine = nullptr;
		if (FAILED(hr = XAudio2Create(&m_Engine, 0, XAUDIO2_DEFAULT_PROCESSOR)))
		{
			logger::ASSERT(false, "Creating XAudio2 failed.");
			logger::log_error("<XAudio2> Creating XAudio2 failed.");
			return;
		}

		m_MasterVoice = nullptr;
		if (FAILED(hr = m_Engine->CreateMasteringVoice(&m_MasterVoice)))
		{
			logger::ASSERT(false, "Creating XAudio2 Mastering Voice failed.");
			logger::log_error("<XAudio2> Creating XAudio2 Mastering Voice failed.");
			return;
		}
	}

	AudioSystem::~AudioSystem()
	{
		m_Active = false;

		m_Channels.clear();

		m_MasterVoice->DestroyVoice();
		m_Engine->Release();

		CoUninitialize();
	}

	/// <summary>
	/// Updates the audio.
	/// </summary>
	void AudioSystem::Update()
	{
		while (m_Active)
			UpdateChannels();
	}

	/// <summary>
	/// Updates the channels.
	/// </summary>
	void AudioSystem::UpdateChannels()
	{
		for (int32_t i = static_cast<int32_t>(ChannelSize() - 1); i > -1; i--)
			if (!m_Channels[i].IsInUse())
				m_Channels.erase(m_Channels.begin() + i);

		if (m_Playback)
			for (int32_t i = static_cast<int32_t>(ChannelSize() - 1); i > -1; i--)
				m_Channels[i].Update();
	}

	/// <summary>
	/// Updates the audio.
	/// </summary>
	void AudioSystem::UpdateNonExtraThread()
	{
		if (m_AudioMode != AUDIO_MODE::AUDIO_MODE_NORMAL)
			return;
		UpdateChannels();
	}

	/// <summary>
	/// Sets the playback status.
	/// </summary>
	/// <param name="a_Playback">The playback status.</param>
	void AudioSystem::SetPlaybackStatus(bool a_Playback)
	{
		m_Playback = a_Playback;
	}

	/// <summary>
	/// Stops the audio thread.
	/// </summary>
	void AudioSystem::Stop()
	{
		m_Active = false;
		if (m_AudioMode == AUDIO_MODE::AUDIO_MODE_THREADED)
			m_Thread.join();
	}

	/// <summary>
	/// Starts the audio thread.
	/// </summary>
	void AudioSystem::Start()
	{
		m_Active = true;
		if (m_AudioMode == AUDIO_MODE::AUDIO_MODE_THREADED)
			m_Thread = std::thread(&AudioSystem::Update, this);
	}

	/// <summary>
	/// Whether the audio system is active.
	/// </summary>
	/// <returns>Whether the audio system is active.</returns>
	bool AudioSystem::HasPlayback() const
	{
		return m_Playback;
	}

	/// <summary>
	/// The XAudio2 Engine.
	/// </summary>
	/// <returns>The XAudio2 Engine.</returns>
	IXAudio2 &AudioSystem::GetEngine() const
	{
		return *m_Engine;
	}

	/// <summary>
	/// Sets the master volume.
	/// </summary>
	/// <param name="a_Volume">The volume.</param>
	void AudioSystem::SetMasterVolume(float a_Volume)
	{
		a_Volume = utils::clamp(a_Volume, UAUDIO_MIN_VOLUME, UAUDIO_MAX_VOLUME);
		m_Volume = a_Volume;
	}

	/// <summary>
	/// Returns the master volume.
	/// </summary>
	/// <returns>The volume</returns>
	float AudioSystem::GetMasterVolume() const
	{
		return m_Volume;
	}

	/// <summary>
	/// Sets the master panning.
	/// </summary>
	/// <param name="a_Panning">The panning.</param>
	void AudioSystem::SetMasterPanning(float a_Panning)
	{
		a_Panning = utils::clamp(a_Panning, UAUDIO_MIN_PANNING, UAUDIO_MAX_PANNING);
		m_Panning = a_Panning;
	}

	/// <summary>
	/// Returns the master panning.
	/// </summary>
	/// <returns>The panning.</returns>
	float AudioSystem::GetMasterPanning() const
	{
		return m_Panning;
	}

	/// <summary>
	/// Returns the buffer size.
	/// </summary>
	/// <returns>Returns the buffer size.</returns>
	BUFFERSIZE AudioSystem::GetBufferSize() const
	{
		return m_BufferSize;
	}

	/// <summary>
	/// Sets the buffer size.
	/// </summary>
	/// <param name="a_BufferSize">The buffer size for every channel.</param>
	void AudioSystem::SetBufferSize(BUFFERSIZE a_BufferSize)
	{
		m_BufferSize = a_BufferSize;
	}

	/// <summary>
	/// Makes a sound play.
	/// </summary>
	/// <param name="a_WaveFile">The sound that needs to be played.</param>
	/// <returns>Channel handle.</returns>
	ChannelHandle AudioSystem::Play(const WaveFile &a_WaveFile)
	{
		// First look for inactive channels.
		for (uint32_t i = 0; i < ChannelSize(); i++)
		{
			if (!m_Channels[i].IsInUse())
			{
				m_Channels[i].SetSound(a_WaveFile);
				m_Channels[i].Play();
				return static_cast<int32_t>(i);
			}
		}

		if (ChannelSize() < UAUDIO_DEFAULT_NUM_CHANNELS)
		{
			const int32_t size = static_cast<int32_t>(ChannelSize());
			xaudio2::XAudio2Channel channel = xaudio2::XAudio2Channel(*this);
			channel.SetSound(a_WaveFile);
			channel.Play();
			m_Channels.push_back(channel);
			return static_cast<int32_t>(size);
		}
		logger::log_warning("<XAudio2> No inactive channels detected.");
		return SOUND_NULL_HANDLE;
	}

	/// <summary>
	/// Returns the amount of channels that are currently added.
	/// </summary>
	/// <returns>The amount of channels.</returns>
	uint32_t AudioSystem::ChannelSize() const
	{
		return static_cast<uint32_t>(m_Channels.size());
	}

	/// <summary>
	/// Returns a channel based on the channel handle.
	/// </summary>
	/// <param name="a_ChannelHandle">Handle to the channel.</param>
	/// <returns>Channel pointer.</returns>
	xaudio2::XAudio2Channel *AudioSystem::GetChannel(ChannelHandle a_ChannelHandle)
	{
		logger::ASSERT(a_ChannelHandle.IsValid() && a_ChannelHandle < static_cast<int32_t>(ChannelSize()), "");

		return &m_Channels[a_ChannelHandle];
	}
}
