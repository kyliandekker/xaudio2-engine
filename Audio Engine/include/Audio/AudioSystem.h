#pragma once

#include <vector>
#include "BasePlayer.h"
#include "SoundHandle.h"
#include "WaveFile.h"

class AudioSystem
{
public:
	AudioSystem();
	AudioSystem(const AudioSystem& rhs);
	virtual ~AudioSystem();

	AudioSystem& operator=(const AudioSystem& rhs);

	Handle CreateSound(const char* a_Path);
	Handle Play(Handle a_SoundHandle);

	void StopChannel(Handle a_ChannelHandle);
	void PlayChannel(Handle a_ChannelHandle);
	void PauseChannel(Handle a_ChannelHandle);

	void Update();
	void Resume();
	void Pause();
	bool IsPlaying();

	const std::vector<WaveFile*>& GetSounds() const;
	const std::vector<BaseChannel*>& GetChannels() const;

	BasePlayer& GetPlayer() const;

	void RemoveSound(Handle a_SoundHandle);
	void PauseAllChannelsWithSound(Handle a_SoundHandle);
	void ResumeAllChannelsWithSound(Handle a_SoundHandle);
private:
	BasePlayer* m_Player = nullptr;
	std::vector<WaveFile*> m_Sounds = std::vector<WaveFile*>();
};
