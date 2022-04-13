#pragma once

#include "ImGuiElement.h"
#include "xaudio2_engine/xaudio2/XAudio2Channel.h"

class ImGuiWaveFile : public ImGuiElement
{
public:
	ImGuiWaveFile(WaveFile& a_WaveFile);
	void RenderElement(uint32_t a_Index) override;
private:
	WaveFile* m_WaveFile = nullptr;
	float m_Tempo = 0;
};