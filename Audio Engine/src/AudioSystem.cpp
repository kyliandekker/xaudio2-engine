#include <AudioSystem.h>

#include <utils/Logger.h>

#include <xaudio2.h>
#include <algorithm>
#include <cassert>

namespace uaudio
{
	AudioSystem::AudioSystem()
	{
		HRESULT hr;
		if (FAILED(hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
		{
			logger::log_error("<XAudio2> Initializing COM library failed.");
			return;
		}

		m_Engine = nullptr;
		if (FAILED(hr = XAudio2Create(&m_Engine, 0, XAUDIO2_DEFAULT_PROCESSOR)))
		{
			logger::log_error("<XAudio2> Creating XAudio failed.");
			return;
		}

		m_MasterVoice = nullptr;
		if (FAILED(hr = m_Engine->CreateMasteringVoice(&m_MasterVoice)))
		{
			logger::log_error("<XAudio2> Creating XAudio Mastering Voice failed.");
			return;
		}
		m_Thread = std::thread(&AudioSystem::Update, this);
	}

	AudioSystem::~AudioSystem()
	{
		m_Active = false;
		m_Thread.join();

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
		while (true)
			if (m_Active)
				for (int32_t i = static_cast<int32_t>(ChannelSize() - 1); i > -1; i--)
					m_Channels[i].Update();
	}

	/// <summary>
	/// Sets the status of the audio system.
	/// </summary>
	/// <param name="a_Enabled">Sets the status of the audio system.</param>
	void AudioSystem::SetActive(bool a_Enabled)
	{
		m_Active = a_Enabled;
	}

	/// <summary>
	/// Whether the audio system is active.
	/// </summary>
	/// <returns>Whether the audio system is active.</returns>
	bool AudioSystem::IsActive() const
	{
		return m_Active;
	}

	/// <summary>
	/// The XAudio2 Engine.
	/// </summary>
	/// <returns>The XAudio2 Engine.</returns>
	IXAudio2& AudioSystem::GetEngine() const
	{
		return *m_Engine;
	}

	/// <summary>
	/// Sets the master volume.
	/// </summary>
	/// <param name="a_Volume">The volume.</param>
	void AudioSystem::SetMasterVolume(float a_Volume)
	{
		a_Volume = std::clamp(a_Volume, 0.0f, 1.0f);
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
		a_Panning = std::clamp(a_Panning, -1.0f, 1.0f);
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
	uint32_t AudioSystem::GetBufferSize() const
	{
		return static_cast<int>(m_BufferSize);
	}

	/// <summary>
	/// Makes a sound play.
	/// </summary>
	/// <param name="a_WaveFile">The sound that needs to be played.</param>
	/// <returns>Channel handle.</returns>
	ChannelHandle AudioSystem::Play(WaveFile& a_WaveFile)
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

		if (ChannelSize() < NUM_CHANNELS)
		{
			const int32_t size = static_cast<int32_t>(ChannelSize());
			xaudio2::XAudio2Channel channel = xaudio2::XAudio2Channel(*this);
			channel.SetSound(a_WaveFile);
			channel.Play();
			m_Channels.push_back(channel);
			return static_cast<int32_t>(size);
		}
		logger::log_error("<XAudio2> No inactive channels detected.");
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
	xaudio2::XAudio2Channel* AudioSystem::GetChannel(ChannelHandle a_ChannelHandle)
	{
		ASSERT(a_ChannelHandle.IsValid() && a_ChannelHandle < static_cast<int32_t>(ChannelSize()), "");

		return &m_Channels[a_ChannelHandle];
	}
}