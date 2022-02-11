#include "Audio/BaseChannel.h"

#include "Audio/Effects.h"

#include "Audio/XAudio2Callback.h"

BaseChannel::BaseChannel(BasePlayer* a_Player) : m_Player(a_Player)
{
	
}

BaseChannel::BaseChannel(const BaseChannel& rhs)
{
	m_CurrentSound = rhs.m_CurrentSound;
	m_Player = rhs.m_Player;
	m_IsPlaying = rhs.IsPlaying();
}

BaseChannel::~BaseChannel() = default;

BaseChannel& BaseChannel::operator=(const BaseChannel& rhs)
{
	if (&rhs != this)
	{
		m_CurrentSound = rhs.m_CurrentSound;
		m_Player = rhs.m_Player;
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

void BaseChannel::SetVolume(float a_Volume)
{
	m_Volume = a_Volume;
}

float BaseChannel::GetVolume() const
{
	return m_Volume;
}

void BaseChannel::SetPanning(float a_Panning)
{
	m_Panning = a_Panning;
}

float BaseChannel::GetPanning() const
{
	return m_Panning;
}

uint32_t BaseChannel::GetCurrentDataPos() const
{
	return m_CurrentPos;
}

uint32_t BaseChannel::GetDataSize() const
{
	return m_CurrentDataSize;
}

unsigned char* BaseChannel::GetData() const
{
	return m_Data;
}

unsigned char* BaseChannel::ApplyEffects(unsigned char* a_Data, uint32_t a_BufferSize)
{
	a_Data = effects::change_volume(a_Data, a_BufferSize, m_Volume);
	a_Data = effects::change_panning(a_Data, a_BufferSize, m_Panning);
	return a_Data;
}

/// <summary>
/// Returns the current sound.
/// </summary>
/// <returns></returns>
const WaveFile& BaseChannel::GetSound() const
{
	return *m_CurrentSound;
}