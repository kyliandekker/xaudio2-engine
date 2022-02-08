#pragma once

#include <Windows.h>
#include <xaudio2.h>

#include <vector>

#include "BasePlayer.h"

class WaveFile;

class XAudio2Player : public BasePlayer
{
public:
	XAudio2Player();
	~XAudio2Player() override;

	int Play(const WaveFile& a_Sound) override;

	void StopChannel(int a_ChannelHandle) override;
	void PlayChannel(int a_ChannelHandle) override;
	void PauseChannel(int a_ChannelHandle) override;
	void StopAllChannels() override;

	void Update() override;
	void Pause() override;
	void Resume() override;
	IXAudio2& GetEngine() const;
private:
	IXAudio2* m_Engine = nullptr;
	IXAudio2MasteringVoice* m_MasterVoice = nullptr;
	int m_NumChannels = 20;
};