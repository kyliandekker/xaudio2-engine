#pragma once

#include <vector>

#include "./xaudio2/XAudio2Channel.h"
#include "./Handle.h"
#include "./wav/WaveFile.h"

constexpr uint32_t NUM_CHANNELS = 20;

enum class BUFFERSIZE
{
	BUFFERSIZE_256 = 256,
	BUFFERSIZE_384 = 384,
	BUFFERSIZE_512 = 512,
	BUFFERSIZE_1024 = 1024,
	BUFFERSIZE_2048 = 2048,
};

class AudioSystem
{
public:
	AudioSystem();
	AudioSystem(const AudioSystem& rhs);
	virtual ~AudioSystem();

	AudioSystem& operator=(const AudioSystem& rhs);

	Handle CreateSound(const char* a_Path);
	void RemoveSound(Handle a_SoundHandle);

	Handle Play(Handle a_SoundHandle);

	void StopAllChannels();
	void PauseAllChannelsWithSound(Handle a_SoundHandle);
	void ResumeAllChannelsWithSound(Handle a_SoundHandle);

	void Update();
	void Pause();
	void Resume();
	bool IsPlaying() const;

	IXAudio2& GetEngine() const;

	void SetVolume(float a_Volume);
	float GetVolume() const;

	void SetPanning(float a_Panning);
	float GetPanning() const;

	uint32_t ChannelSize() const;
	XAudio2Channel* GetChannel(Handle a_ChannelHandle) const;

	uint32_t SoundSize() const;
	WaveFile* GetSound(Handle a_SoundHandle) const;
	uint32_t GetBufferSize();
private:
	IXAudio2* m_Engine = nullptr;
	IXAudio2MasteringVoice* m_MasterVoice = nullptr;

	BUFFERSIZE m_BufferSize = BUFFERSIZE::BUFFERSIZE_2048;

	std::vector<XAudio2Channel*> m_Channels = std::vector<XAudio2Channel*>();

	bool m_IsPlaying = true;
	float m_Volume = 1.0f;
	float m_Panning = 0.0f;

	std::vector<WaveFile*> m_Sounds = std::vector<WaveFile*>();
};
