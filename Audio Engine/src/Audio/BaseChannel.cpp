#include "Audio/BaseChannel.h"

#include "Audio/XAudio2Callback.h"

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

void BaseChannel::SetVolume(float a_Volume)
{
	m_Volume = a_Volume;
}

float BaseChannel::GetVolume()
{
	return m_Volume;
}

unsigned char* BaseChannel::ApplyEffects(unsigned char* a_Data, uint32_t a_BufferSize)
{
	a_Data = ApplyVolume(a_Data, a_BufferSize, m_Volume);
	return a_Data;
}

unsigned char* BaseChannel::ApplyVolume(unsigned char* a_Data, uint32_t a_BufferSize, float a_Volume)
{
	for (int i = 0; i < a_BufferSize; i += 2)
	{
		short wData;
		wData = MAKEWORD(a_Data[i], a_Data[i + 1]);
		long dwData = wData;
		dwData = dwData * a_Volume;
		if (dwData < -0x8000)
		{
			dwData = -0x8000;
		}
		else if (dwData > 0x7FFF)
		{
			dwData = 0x7FFF;
		}
		wData = LOWORD(dwData);
		a_Data[i] = LOBYTE(wData);
		a_Data[i + 1] = HIBYTE(wData);
	}

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