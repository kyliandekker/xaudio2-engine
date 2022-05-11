#include <uaudio/system/low_level/backends/xaudio2/XAudio2Backend.h>

#include <uaudio/utils/Logger.h>

#include <uaudio/system/high_level/SoundSystem.h>

namespace uaudio::xaudio2
{
	XAudio2Backend::XAudio2Backend()
	{
		HRESULT hr;
		if (FAILED(hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
		{
			logger::ASSERT(false, "<XAudio2> Initializing COM library failed.");
			return;
		}

		m_Engine = nullptr;
		if (FAILED(hr = XAudio2Create(&m_Engine, 0, XAUDIO2_DEFAULT_PROCESSOR)))
		{
			logger::ASSERT(false, "<XAudio2> Creating XAudio2 failed.");
			return;
		}

		m_MasterVoice = nullptr;
		if (FAILED(hr = m_Engine->CreateMasteringVoice(&m_MasterVoice)))
		{
			logger::ASSERT(false, "<XAudio2> Creating XAudio2 Mastering Voice failed.");
			return;
		}
	}

	XAudio2Backend::~XAudio2Backend()
	{
		m_Channels.clear();

		m_MasterVoice->DestroyVoice();
		m_Engine->Release();

		CoUninitialize();
	}

	/// <summary>
	/// Returns the XAudio2 Engine.
	/// </summary>
	/// <returns></returns>
	IXAudio2* XAudio2Backend::GetEngine() const
	{
		return m_Engine;
	}

	/// <summary>
	/// Creates a channel with the sound attached.
	/// </summary>
	/// <param name="a_SoundData">The sound data for the channel.</param>
	/// <returns></returns>
	int32_t XAudio2Backend::CreateSoundChannel(SoundData& a_SoundData)
	{
		WaveFormat* wave_format = nullptr;
		SoundSys.FindSound(wave_format, a_SoundData.soundHash);

		const size_t size = CreateSoundChannel();
		if (wave_format != nullptr)
			m_Channels[size].SetSound(*wave_format, a_SoundData);

		return static_cast<int32_t>(size);
	}

	int32_t XAudio2Backend::CreateSoundChannel()
	{
		for (size_t i = 0; i < m_Channels.size(); i++)
		{
			bool is_in_use = false;
			m_Channels[i].IsInUse(is_in_use);
			if (!is_in_use)
				return static_cast<int32_t>(i);
		}
		const size_t size = m_Channels.size();
		m_Channels.push_back(XAudio2Channel(*this));
		return static_cast<int32_t>(size);
	}

	/// <summary>
	/// Updates the audio system.
	/// </summary>
	void XAudio2Backend::Update()
	{
		if (m_Playback)
			for (int32_t i = static_cast<int32_t>(m_Channels.size() - 1); i > -1; i--)
				m_Channels[i].Update();
	}

	Channel* XAudio2Backend::GetChannel(int32_t a_Index)
	{
		return &m_Channels[a_Index];
	}
}
