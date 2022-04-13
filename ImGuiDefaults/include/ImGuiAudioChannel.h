#pragma once

#include "ImGuiElement.h"
#include "xaudio2_engine/xaudio2/XAudio2Channel.h"

class ImGuiAudioChannel : public ImGuiElement
{
public:
	ImGuiAudioChannel(AudioSystem& a_AudioSystem);
	ImGuiAudioChannel(XAudio2Channel& a_Channel);
	void RenderElement(uint32_t a_Index) override;
	void Update();
private:
	XAudio2Channel* m_Channel = nullptr;
};