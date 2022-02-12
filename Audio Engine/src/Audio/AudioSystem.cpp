#include "Audio/AudioSystem.h"

#include "Audio/Logger.h"
#include "Audio/math.h"

#include <xaudio2.h>

AudioSystem::AudioSystem()
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

AudioSystem::AudioSystem(const AudioSystem &rhs)
{
	m_Sounds = rhs.m_Sounds;
	m_Channels = rhs.m_Channels;

	m_IsPlaying = rhs.m_IsPlaying;
	m_Engine = rhs.m_Engine;
	m_MasterVoice = rhs.m_MasterVoice;
	m_Volume = rhs.m_Volume;
	m_Panning = rhs.m_Panning;
}

AudioSystem::~AudioSystem()
{
	for (int32_t i = static_cast<int32_t>(ChannelSize()) - 1; i > -1; i--)
		delete m_Channels[i];
	m_Channels.clear();

	for (int32_t i = static_cast<int32_t>(SoundSize()) - 1; i > -1; i--)
		RemoveSound(1);
	m_Sounds.clear();

	m_MasterVoice->DestroyVoice();
	m_Engine->Release();
	CoUninitialize();
}

AudioSystem &AudioSystem::operator=(const AudioSystem &rhs)
{
	if (&rhs != this)
	{
		m_Sounds = rhs.m_Sounds;
		m_Channels = rhs.m_Channels;

		m_IsPlaying = rhs.m_IsPlaying;
		m_Engine = rhs.m_Engine;
		m_MasterVoice = rhs.m_MasterVoice;
		m_Volume = rhs.m_Volume;
		m_Panning = rhs.m_Panning;
	}
	return *this;
}

/// <summary>
/// Adds a sound to the database and returns a handle to the sound.
/// </summary>
/// <param name="a_Path">The path to the sound.</param>
/// <returns>Sound handle.</returns>
Handle AudioSystem::CreateSound(const char *a_Path)
{
	logger::log_info("<XAudio2> Creating sound: \"%s\".", a_Path);
	const Handle index = SoundSize();
	m_Sounds.push_back(new WaveFile(a_Path));
	return index;
}

/// <summary>
/// Removes a sound from the database.
/// </summary>
/// <param name="a_SoundHandle">Handle to the sound that needs to be deleted.</param>
void AudioSystem::RemoveSound(Handle a_SoundHandle)
{
	for (int32_t i = static_cast<int32_t>(ChannelSize()) - 1; i > -1; i--)
	{
		XAudio2Channel* channel = GetChannel(i);
		if (channel != nullptr)
			if (&channel->GetSound() == m_Sounds[a_SoundHandle])
				channel->RemoveSound();
	}
	delete m_Sounds[a_SoundHandle];
	m_Sounds.erase(m_Sounds.begin() + a_SoundHandle);
}

/// <summary>
/// Makes a sound play.
/// </summary>
/// <param name="a_SoundHandle">Handle to the sound that needs to be played.</param>
/// <returns>Channel handle.</returns>
Handle AudioSystem::Play(Handle a_SoundHandle)
{
	if (!a_SoundHandle.IsValid() || a_SoundHandle >= SoundSize())
		return SOUND_NULL_HANDLE;

	const WaveFile &sound = *m_Sounds[a_SoundHandle];

	// First look for inactive channels.
	for (uint32_t i = 0; i < static_cast<uint32_t>(ChannelSize()); i++)
	{
		if (!m_Channels[i]->IsInUse())
		{
			m_Channels[i]->SetSound(sound);
			m_Channels[i]->Start();
			return i;
		}
	}

	if (ChannelSize() < NUM_CHANNELS)
	{
		const uint32_t size = ChannelSize();
		XAudio2Channel *channel = new XAudio2Channel(*this);
		m_Channels.push_back(channel);
		channel->SetSound(sound);
		channel->Start();
		return size;
	}
	logger::log_error("<XAudio> No inactive channels detected.");
	return SOUND_NULL_HANDLE;
}

/// <summary>
/// Stops all the channels and resets them (removing the sound and playback data).
/// </summary>
void AudioSystem::StopAllChannels()
{
	for (int32_t i = static_cast<int32_t>(ChannelSize()) - 1; i > -1; i--)
	{
		XAudio2Channel* channel = GetChannel(i);
		if (channel != nullptr)
			channel->Stop();
	}
}

/// <summary>
/// Pauses he playback of all the channels that share the sound handle.
/// </summary>
/// <param name="a_SoundHandle">Handle to the sound.</param>
void AudioSystem::PauseAllChannelsWithSound(Handle a_SoundHandle)
{
	for (int32_t i = static_cast<int32_t>(ChannelSize()) - 1; i > -1; i--)
	{
		XAudio2Channel* channel = GetChannel(i);
		if (channel != nullptr)
			if (&channel->GetSound() == m_Sounds[a_SoundHandle])
				channel->Pause();
	}
}

/// <summary>
/// Resumes the playback of all the channels that share the sound handle.
/// </summary>
/// <param name="a_SoundHandle">Handle to the channel that needs to be resumed.</param>
void AudioSystem::ResumeAllChannelsWithSound(Handle a_SoundHandle)
{
	for (int32_t i = static_cast<int32_t>(ChannelSize()) - 1; i > -1; i--)
	{
		XAudio2Channel* channel = GetChannel(i);
		if (channel != nullptr)
			if (&channel->GetSound() == m_Sounds[a_SoundHandle])
				channel->Resume();
	}
}

/// <summary>
/// Updates the audio.
/// </summary>
void AudioSystem::Update()
{
	if (m_IsPlaying)
		for (int32_t i = static_cast<int32_t>(ChannelSize()) - 1; i > -1; i--)
			m_Channels[i]->Update();
}

/// <summary>
/// Pauses the playback of the audio system.
/// </summary>
void AudioSystem::Pause()
{
	m_IsPlaying = false;
}

/// <summary>
/// Resumes the playback of the audio system.
/// </summary>
void AudioSystem::Resume()
{
	m_IsPlaying = true;
}

/// <summary>
/// Whether the playback is resumed.
/// </summary>
/// <returns>Whether the playback is resumed.</returns>
bool AudioSystem::IsPlaying() const
{
	return m_IsPlaying;
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
void AudioSystem::SetVolume(float a_Volume)
{
	a_Volume = math::ClampF(a_Volume, 0.0f, 1.0f);
	m_Volume = a_Volume;
}

/// <summary>
/// Returns the master volume.
/// </summary>
/// <returns>The volume</returns>
float AudioSystem::GetVolume() const
{
	return m_Volume;
}

/// <summary>
/// Sets the master panning.
/// </summary>
/// <param name="a_Panning">The panning.</param>
void AudioSystem::SetPanning(float a_Panning)
{
	a_Panning = math::ClampF(a_Panning, -1.0f, 1.0f);
	m_Panning = a_Panning;
}

/// <summary>
/// Returns the master panning.
/// </summary>
/// <returns>The panning.</returns>
float AudioSystem::GetPanning() const
{
	return m_Panning;
}

/// <summary>
/// Returns the amount of channels that are currently added.
/// </summary>
/// <returns>The amount of channels.</returns>
int32_t AudioSystem::ChannelSize() const
{
	return static_cast<int32_t>(m_Channels.size());
}

/// <summary>
/// Returns a channel based on the channel handle.
/// </summary>
/// <param name="a_ChannelHandle">Handle to the channel.</param>
/// <returns>Channel pointer.</returns>
XAudio2Channel *AudioSystem::GetChannel(Handle a_ChannelHandle) const
{
	if (!a_ChannelHandle.IsValid() || a_ChannelHandle >= ChannelSize())
		return nullptr;

	return m_Channels[a_ChannelHandle];
}

/// <summary>
/// Returns the amount of sounds in the database.
/// </summary>
/// <returns>The amount of sounds.</returns>
int32_t AudioSystem::SoundSize() const
{
	return static_cast<int32_t>(m_Sounds.size());
}

/// <summary>
/// Returns a sound based on the sound handle.
/// </summary>
/// <param name="a_SoundHandle">Handle to the sound.</param>
/// <returns>Sound pointer.</returns>
WaveFile *AudioSystem::GetSound(Handle a_SoundHandle) const
{
	if (!a_SoundHandle.IsValid() || a_SoundHandle >= SoundSize())
		return nullptr;

	return m_Sounds[a_SoundHandle];
}
