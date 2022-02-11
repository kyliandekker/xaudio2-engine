#pragma once

#include "WaveFile.h"

class BasePlayer;

class BaseChannel
{
public:
	BaseChannel(BasePlayer* a_Player);
	BaseChannel(const BaseChannel& rhs);
	virtual ~BaseChannel();

	BaseChannel& operator=(const BaseChannel& rhs);

	virtual void Resume() = 0;
	virtual void Pause() = 0;
	virtual void Update() = 0;
	virtual void Stop() = 0;
	virtual void Reset() = 0;
	virtual void RemoveSound() = 0;
	virtual bool IsInUse() const;
	virtual bool IsPlaying() const;

	void SetVolume(float a_Volume);
	float GetVolume() const;

	void SetPanning(float a_Panning);
	float GetPanning() const;

	uint32_t GetCurrentDataPos() const;
	uint32_t GetDataSize() const;
	unsigned char* GetData() const;

	unsigned char* ApplyEffects(unsigned char* a_Data, uint32_t a_BufferSize);

	const WaveFile& GetSound() const;
protected:
	float m_Volume = 1;
	float m_Panning = 0.0f;
	const WaveFile* m_CurrentSound = nullptr;
	bool m_IsPlaying = false;
	BasePlayer* m_Player = nullptr;

	uint32_t m_CurrentPos = 0;
	uint32_t m_CurrentDataSize = 0;

	unsigned char* m_Data = nullptr;
};
