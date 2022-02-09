#pragma once

#include "WaveFile.h"

class BaseChannel
{
public:
	BaseChannel();
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
	float GetVolume();

	unsigned char* ApplyEffects(unsigned char* a_Data, uint32_t a_BufferSize);

	const WaveFile& GetSound() const;
protected:
	unsigned char* ApplyVolume(unsigned char* a_Data, uint32_t a_BufferSize, float a_Volume);

	float m_Volume = 1;
	const WaveFile* m_CurrentSound = nullptr;
	bool m_IsPlaying = false;
};
