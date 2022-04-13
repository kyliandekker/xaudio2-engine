#pragma once

#include <xaudio2.h>

#include "./XAudio2Callback.h"

class WaveFile;
class AudioSystem;

class XAudio2Channel
{
	enum class TIMEUNIT
	{
		TIMEUNIT_MS,
		TIMEUNIT_S,
		TIMEUNIT_POS
	};
public:
	XAudio2Channel(AudioSystem& a_AudioSystem);
	XAudio2Channel(const XAudio2Channel& rhs);
	~XAudio2Channel();

	XAudio2Channel& operator=(const XAudio2Channel& rhs);

	void SetSound(const WaveFile& a_Sound);

	void Start();
	void Resume();
	void Pause();
	void Update();
	void SetPos(uint32_t a_StartPos);
	float GetPos(TIMEUNIT a_TimeUnit) const;
	void PlayRanged(uint32_t a_StartPos, uint32_t a_Size);
	void Stop();
	void Reset();
	void RemoveSound();

	IXAudio2SourceVoice& GetSourceVoice() const;
	XAudio2Callback& GetVoiceCallback();

	void SetVolume(float a_Volume);
	float GetVolume() const;

	void SetPanning(float a_Panning);
	float GetPanning() const;

	bool IsPlaying() const;
	bool IsInUse() const;

	uint32_t GetDataSize() const;
	unsigned char* GetData() const;

	unsigned char* ApplyEffects(unsigned char* a_Data, uint32_t a_BufferSize);

	const WaveFile& GetSound() const;
private:
	float m_Volume = 1;
	float m_Panning = 0.0f;
	const WaveFile* m_CurrentSound = nullptr;
	bool m_IsPlaying = false;
	AudioSystem& m_AudioSystem;

	uint32_t m_CurrentPos = 0;
	uint32_t m_DataSize = 0;

	float m_Tick = 0.0f;
	float m_Ticks = 0.0f;

	unsigned char* m_Data = nullptr;

	IXAudio2SourceVoice* m_SourceVoice = nullptr;
	XAudio2Callback m_VoiceCallback;
};