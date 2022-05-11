#pragma once

#include "tools/BaseTool.h"
#include <uaudio/system/high_level/AudioSystem.h>

class ChannelsTool : public BaseTool
{
public:
	ChannelsTool(uaudio::AudioSystem& a_AudioSystem);
	void Render() override;
private:
	void RenderChannel(uint32_t a_Index, uaudio::AudioComponent& a_Component);

	uaudio::AudioSystem& m_AudioSystem;
};
