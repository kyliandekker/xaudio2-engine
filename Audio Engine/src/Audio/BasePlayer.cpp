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

const std::vector<BaseChannel *> &BasePlayer::GetChannels() const
{
	return m_Channels;
}