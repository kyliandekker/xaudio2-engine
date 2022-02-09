#include "Audio/BasePlayer.h"

BasePlayer::BasePlayer() = default;

BasePlayer::BasePlayer(const BasePlayer &rhs)
{
	m_IsPlaying = rhs.m_IsPlaying;
}

BasePlayer::~BasePlayer()
{
	for (const auto& m_Channel : m_Channels)
		delete m_Channel;
	m_Channels.clear();
}

BasePlayer &BasePlayer::operator=(const BasePlayer &rhs)
{
	if (&rhs != this)
	{
		m_IsPlaying = rhs.m_IsPlaying;
	}
	return *this;
}

bool BasePlayer::IsPlaying() const
{
	return m_IsPlaying;
}

void BasePlayer::SetVolume(float a_Volume)
{
	m_Volume = a_Volume;
}

float BasePlayer::GetVolume() const
{
	return m_Volume;
}

void BasePlayer::SetPanning(float a_Panning)
{
	m_Panning = a_Panning;
}

float BasePlayer::GetPanning() const
{
	return m_Panning;
}

const std::vector<BaseChannel *> &BasePlayer::GetChannels() const
{
	return m_Channels;
}