#pragma once

#include "AudioSystem.h"
#include "tools/BaseTool.h"

class ChannelsTool : public BaseTool
{
public:
	ChannelsTool(uaudio::AudioSystem& a_AudioSystem);
	void Render() override;
private:
	void RenderChannel(uint32_t a_Index, uaudio::xaudio2::XAudio2Channel* a_Channel);

	uaudio::AudioSystem& m_AudioSystem;
};
