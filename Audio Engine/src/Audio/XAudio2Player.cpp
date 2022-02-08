#include "Audio/WaveFile.h"
#include "Audio/XAudio2Player.h"

#include "Audio/Logger.h"
#include "Audio/XAudio2Channel.h"

XAudio2Player::XAudio2Player() : BasePlayer()
{
	HRESULT hr;
	if (FAILED(hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		logger::log_error("<XAudio> Initializing COM library failed.");
		return;
	}

	m_Engine = nullptr;
	if (FAILED(hr = XAudio2Create(&m_Engine, 0, XAUDIO2_DEFAULT_PROCESSOR)))
	{
		logger::log_error("<XAudio> Creating XAudio failed.");
		return;
	}

	m_MasterVoice = nullptr;
	if (FAILED(hr = m_Engine->CreateMasteringVoice(&m_MasterVoice)))
	{
		logger::log_error("<XAudio> Creating XAudio Mastering Voice failed.");
		return;
	}
}

XAudio2Player::~XAudio2Player()
{
	for (int i = static_cast<int>(m_Channels.size()) - 1; i > -1; i--)
		delete m_Channels[i];
	m_Channels.clear();
	m_Engine->Release();
	CoUninitialize();
}

/// <summary>
/// Looks for an inactive channel and sets its sound.
/// </summary>
/// <param name="a_Sound"></param>
int XAudio2Player::Play(const WaveFile &a_Sound)
{
	for (int i = 0; i < static_cast<int>(m_Channels.size()); i++)
		if (!m_Channels[i]->IsInUse())
		{
			reinterpret_cast<XAudio2Channel *>(m_Channels[i])->SetSound(a_Sound);
			return i;
		}
	if (static_cast<int>(m_Channels.size()) < m_NumChannels)
	{
		XAudio2Channel *channel = new XAudio2Channel(*this);
		m_Channels.push_back(channel);
		channel->SetSound(a_Sound);
		return static_cast<int>(m_Channels.size()) - 1;
	}
	logger::log_error("<XAudio> No inactive channels detected.");
	return -1;
}

/// <summary>
/// Stops a channel from playing.
/// </summary>
/// <param name="a_ChannelHandle"></param>
void XAudio2Player::StopChannel(int a_ChannelHandle)
{
	if (a_ChannelHandle < 0 || a_ChannelHandle >= static_cast<int>(m_Channels.size()))
		return;
	m_Channels[a_ChannelHandle]->Stop();
	m_Channels[a_ChannelHandle]->Reset();
}

/// <summary>
/// Resumes playback on a channel.
/// </summary>
/// <param name="a_ChannelHandle"></param>
void XAudio2Player::PlayChannel(int a_ChannelHandle)
{
	m_Channels[a_ChannelHandle]->Play();
}

/// <summary>
/// Stops playback on a channel.
/// </summary>
/// <param name="a_ChannelHandle"></param>
void XAudio2Player::PauseChannel(int a_ChannelHandle)
{
	m_Channels[a_ChannelHandle]->Pause();
}

/// <summary>
/// Stops all channels from playing.
/// </summary>
void XAudio2Player::StopAllChannels()
{
	for (int i = 0; i < static_cast<int>(m_Channels.size()); i++)
		if (m_Channels[i]->IsInUse())
			m_Channels[i]->Stop();
}

/// <summary>
/// Updates all the channels and sounds (called per frame).
/// </summary>
void XAudio2Player::Update()
{
	if (m_IsPlaying)
		for (int i = 0; i < static_cast<int>(m_Channels.size()); i++)
			if (m_Channels[i]->IsInUse())
				m_Channels[i]->Update();
}

/// <summary>
/// Stops the playback.
/// </summary>
void XAudio2Player::Pause()
{
	m_IsPlaying = false;
}

/// <summary>
/// Resumes the playback.
/// </summary>
void XAudio2Player::Resume()
{
	m_IsPlaying = true;
}

/// <summary>
/// Returns the XAudio2 engine.
/// </summary>
/// <returns></returns>
IXAudio2 &XAudio2Player::GetEngine() const
{
	return *m_Engine;
}