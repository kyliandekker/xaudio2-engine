#include "Audio/AudioSystem.h"
#include "Audio/XAudio2Player.h"

#include "Audio/Logger.h"

AudioSystem::AudioSystem()
{
	m_Player = new XAudio2Player();
}

AudioSystem::AudioSystem(const AudioSystem &rhs)
{
	m_Player = rhs.m_Player;
	m_Sounds = rhs.m_Sounds;
}

AudioSystem::~AudioSystem()
{
	for (int i = 0; i < static_cast<int>(m_Sounds.size()); i++)
		RemoveSound(1);
	delete m_Player;
}

AudioSystem &AudioSystem::operator=(const AudioSystem &rhs)
{
	if (&rhs != this)
	{
		m_Player = rhs.m_Player;
		m_Sounds = rhs.m_Sounds;
	}
	return *this;
}

/// <summary>
/// Creates a sound and adds it to the list of sounds.
/// </summary>
/// <param name="a_Path">The path of where the sound resides.</param>
/// <returns></returns>
Handle AudioSystem::CreateSound(const char* a_Path)
{
	logger::log_info("<XAudio2> Creating sound: \"%s\".", a_Path);
	const Handle index = static_cast<int>(m_Sounds.size());
	m_Sounds.push_back(new WaveFile(a_Path));
	return index;
}

/// <summary>
/// Plays a sound.
/// </summary>
/// <param name="a_SoundHandle">The index of the sound (stored in the sound list)</param>
/// <returns></returns>
Handle AudioSystem::Play(Handle a_SoundHandle)
{
	return m_Player->Play(*m_Sounds[a_SoundHandle]);
}

/// <summary>
/// Stops playback on a channel.
/// </summary>
/// <param name="a_ChannelHandle"></param>
void AudioSystem::StopChannel(Handle a_ChannelHandle)
{
	m_Player->StopChannel(a_ChannelHandle);
}

/// <summary>
/// Resumes playback on a channel.
/// </summary>
/// <param name="a_ChannelHandle"></param>
void AudioSystem::PlayChannel(Handle a_ChannelHandle)
{
	m_Player->PlayChannel(a_ChannelHandle);
}

/// <summary>
/// Stops playback on a channel.
/// </summary>
/// <param name="a_ChannelHandle"></param>
void AudioSystem::PauseChannel(Handle a_ChannelHandle)
{
	m_Player->PauseChannel(a_ChannelHandle);
}

/// <summary>
/// Updates all the channels and sounds (called per frame).
/// </summary>
void AudioSystem::Update()
{
	m_Player->Update();
}

/// <summary>
/// Resumes the playback.
/// </summary>
void AudioSystem::Resume()
{
	m_Player->Resume();
}

/// <summary>
/// Stops the playback.
/// </summary>
void AudioSystem::Pause()
{
	m_Player->Pause();
}

/// <summary>
/// Returns the state of the playback.
/// </summary>
/// <returns></returns>
bool AudioSystem::IsPlaying()
{
	return m_Player->IsPlaying();
}

const std::vector<WaveFile *> &AudioSystem::GetSounds() const
{
	return m_Sounds;
}

const std::vector<BaseChannel *> &AudioSystem::GetChannels() const
{
	return m_Player->GetChannels();
}

BasePlayer& AudioSystem::GetPlayer() const
{
	return *m_Player;
}

/// <summary>
/// Removes a sound from all the channels using it.
/// </summary>
/// <param name="a_SoundHandle"></param>
void AudioSystem::RemoveSound(Handle a_SoundHandle)
{
	for (const auto i : m_Player->GetChannels())
		if (&i->GetSound() == m_Sounds[a_SoundHandle])
			i->RemoveSound();
	delete m_Sounds[a_SoundHandle];
	m_Sounds.erase(m_Sounds.begin() + a_SoundHandle);
}

/// <summary>
/// Pauses a sound in all the channels that use it.
/// </summary>
/// <param name="a_SoundHandle"></param>
void AudioSystem::PauseAllChannelsWithSound(Handle a_SoundHandle)
{
	for (const auto i : m_Player->GetChannels())
		if (&i->GetSound() == m_Sounds[a_SoundHandle])
			i->Pause();
}

/// <summary>
/// Resumes a sound in all the channels that use it.
/// </summary>
/// <param name="a_SoundHandle"></param>
void AudioSystem::ResumeAllChannelsWithSound(Handle a_SoundHandle)
{
	for (const auto i : m_Player->GetChannels())
		if (&i->GetSound() == m_Sounds[a_SoundHandle])
			i->Resume();
}