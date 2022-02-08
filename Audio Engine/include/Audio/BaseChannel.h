#pragma once

#include "WaveFile.h"

class BaseChannel
{
public:
	BaseChannel();
	BaseChannel(const BaseChannel& rhs);
	virtual ~BaseChannel();

	BaseChannel& operator=(const BaseChannel& rhs);

	virtual void Play() = 0;
	virtual void Pause() = 0;
	virtual void Update() = 0;
	virtual void Stop() = 0;
	virtual void Reset() = 0;
	virtual void RemoveSound() = 0;
	virtual bool IsInUse() const;
	virtual bool IsPlaying() const;

	const WaveFile& GetSound() const;
protected:
	const WaveFile* m_CurrentSound = nullptr;
	bool m_IsPlaying = false;
};
