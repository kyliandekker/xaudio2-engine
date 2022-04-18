#pragma once

#include "AudioSystem.h"
#include "SoundSystem.h"
#include "tools/BaseTool.h"

class SoundsTool : public BaseTool
{
public:
	SoundsTool(uaudio::AudioSystem& a_AudioSystem, uaudio::SoundSystem& a_SoundSystem);
	void Render() override;
private:
	void RenderSound(uaudio::Hash a_SoundHash);

	uaudio::AudioSystem& m_AudioSystem;
	uaudio::SoundSystem& m_SoundSystem;
};
