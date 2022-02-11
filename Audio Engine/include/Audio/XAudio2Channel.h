#pragma once

#include <xaudio2.h>

#include "XAudio2Callback.h"
#include "BaseChannel.h"

class WaveFile;
class XAudio2Player;

class XAudio2Channel : public BaseChannel
{
public:
	XAudio2Channel(XAudio2Player& a_SoundSystem);
	~XAudio2Channel() override;
	void SetSound(const WaveFile& a_Sound);

	void Resume() override;
	void Pause() override;
	void Update() override;
	void Stop() override;
	void Reset() override;
	void RemoveSound() override;

	IXAudio2SourceVoice& GetSourceVoice() const;
	XAudio2Callback& GetVoiceCallback();

	uint32_t GetCurrentPos() const;
private:
	IXAudio2SourceVoice* m_SourceVoice = nullptr;
	XAudio2Callback m_VoiceCallback;
};