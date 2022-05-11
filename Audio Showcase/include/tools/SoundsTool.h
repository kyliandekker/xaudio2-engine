#pragma once

#include <uaudio/wave/low_level/WaveFormat.h>
#include <uaudio/system/high_level/AudioSystem.h>
#include <uaudio/system/high_level/Channel.h>

#include "tools/BaseTool.h"

class SoundsTool : public BaseTool
{
public:
	SoundsTool(uaudio::AudioSystem &a_AudioSystem);
	void Render() override;
private:
	void RenderSound(uaudio::UAUDIO_DEFAULT_HASH a_SoundHash);
	void RenderPlot(uaudio::UAUDIO_DEFAULT_HASH a_SoundHash);

	void SaveFile(uaudio::WaveFormat* a_WaveFile);

	uaudio::AudioSystem &m_AudioSystem;
	int32_t m_ChannelIndex = -1;
};
