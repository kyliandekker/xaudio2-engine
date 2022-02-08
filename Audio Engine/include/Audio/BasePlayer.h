#pragma once

#include <vector>

#include "BaseChannel.h"
#include "WaveFile.h"

class BasePlayer
{
public:
	BasePlayer();
	BasePlayer(const BasePlayer& rhs);
	virtual ~BasePlayer();

	BasePlayer& operator=(const BasePlayer& rhs);

	bool IsPlaying() const;

	virtual int Play(const WaveFile& a_Sound) = 0;

	virtual void StopAllChannels() = 0;
	virtual void StopChannel(int a_Channel) = 0;
	virtual void PauseChannel(int a_Channel) = 0;
	virtual void PlayChannel(int a_Channel) = 0;

	virtual void Update() = 0;
	virtual void Pause() = 0;
	virtual void Resume() = 0;

	const std::vector<BaseChannel*>& GetChannels() const;
protected:
	std::vector<BaseChannel*> m_Channels = std::vector<BaseChannel*>();
	bool m_IsPlaying = true;
};
