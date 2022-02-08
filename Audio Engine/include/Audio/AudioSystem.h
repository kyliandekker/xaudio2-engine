#pragma once

#include <vector>
#include "BasePlayer.h"
#include "WaveFile.h"

class AudioSystem
{
public:
	AudioSystem();
	AudioSystem(const AudioSystem& rhs);
	virtual ~AudioSystem();

	AudioSystem& operator=(const AudioSystem& rhs);

	int CreateSound(const char* a_Path);
	int Play(int a_SoundHandle);

	void StopChannel(int a_ChannelHandle);
	void PlayChannel(int a_ChannelHandle);
	void PauseChannel(int a_ChannelHandle);

	void Update();
	void Resume();
	void Pause();
	bool IsPlaying();

	const std::vector<WaveFile*>& GetSounds() const;
	const std::vector<BaseChannel*>& GetChannels() const;
	void RemoveSound(int a_Index);
	void StopAllChannelsWithSound(int a_Index);
private:
	BasePlayer* m_Player = nullptr;
	std::vector<WaveFile*> m_Sounds = std::vector<WaveFile*>();
};
