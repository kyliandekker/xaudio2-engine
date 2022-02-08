#include "Audio/BaseChannel.h"

BaseChannel::BaseChannel() = default;

BaseChannel::BaseChannel(const BaseChannel& rhs)
{
	m_CurrentSound = rhs.m_CurrentSound;
	m_IsPlaying = rhs.IsPlaying();
}

BaseChannel::~BaseChannel() = default;

BaseChannel& BaseChannel::operator=(const BaseChannel& rhs)
{
	if (&rhs != this)
	{
		m_CurrentSound = rhs.m_CurrentSound;
		m_IsPlaying = rhs.IsPlaying();
	}
	return *this;
}

/// <summary>
/// Returns whether the current channel is in use.
/// </summary>
/// <returns></returns>
bool BaseChannel::IsInUse() const
{
	return m_CurrentSound != nullptr;
}

bool BaseChannel::IsPlaying() const
{
	return m_IsPlaying;
}

/// <summary>
/// Returns the current sound.
/// </summary>
/// <returns></returns>
const WaveFile& BaseChannel::GetSound() const
{
	return *m_CurrentSound;
}