#pragma once

#include "AudioSystem.h"
#include "SoundSystem.h"
#include "tools/BaseTool.h"

class SoundsTool : public BaseTool
{
public:
	SoundsTool(uaudio::AudioSystem &a_AudioSystem, uaudio::SoundSystem &a_SoundSystem);
	void Render() override;

	static void SaveFile(const uaudio::WaveFile* a_WaveFile);

private:
	void RenderSound(uaudio::UAUDIO_DEFAULT_HASH a_SoundHash);

	uaudio::AudioSystem &m_AudioSystem;
	uaudio::SoundSystem &m_SoundSystem;
	uaudio::xaudio2::XAudio2Channel m_Channel;
};
