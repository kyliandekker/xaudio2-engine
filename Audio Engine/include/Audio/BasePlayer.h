#pragma once

#include <vector>

#include "BaseChannel.h"
#include "SoundHandle.h"
#include "WaveFile.h"

class BasePlayer
{
public:
	BasePlayer();
	BasePlayer(const BasePlayer& rhs);
	virtual ~BasePlayer();

	BasePlayer& operator=(const BasePlayer& rhs);

	bool IsPlaying() const;

	virtual Handle Play(const WaveFile& a_Sound) = 0;

	virtual void StopAllChannels() = 0;
	virtual void StopChannel(Handle a_Channel) = 0;
	virtual void PauseChannel(Handle a_Channel) = 0;
	virtual void PlayChannel(Handle a_Channel) = 0;

	virtual void Update() = 0;
	virtual void Pause() = 0;
	virtual void Resume() = 0;

	void SetVolume(float a_Volume);
	float GetVolume() const;

	void SetPanning(float a_Panning);
	float GetPanning() const;

	const std::vector<BaseChannel*>& GetChannels() const;
protected:
	std::vector<BaseChannel*> m_Channels = std::vector<BaseChannel*>();
	bool m_IsPlaying = true;
	float m_Volume = 1.0f;
	float m_Panning = 0.0f;
};