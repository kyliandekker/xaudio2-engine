#pragma once

#include <imgui/backends/imgui_impl_sdl.h>
#include <sdl/SDL_video.h>

#include <ImguiWindowStruct.h>
#include <AudioSystem.h>

#include "SoundSystem.h"

#define IMGUI_INDENT 16.0f

class AudioImGuiWindow
{
public:
	AudioImGuiWindow(SDL_Window &a_Window, uaudio::AudioSystem &a_AudioSystem);
	uint32_t CreateImGui(const SDL_GLContext &a_Context, const char *a_Glslversion) const;
	float GetRGBColor(int color);
	void RenderImGui();
	void ShowValue(const char *a_Text, const char *a_Value);
	void RenderSound(uaudio::WaveFile& a_WaveFile);
	void RenderChannel(uint32_t a_Index, uaudio::xaudio2::XAudio2Channel* a_Channel);
	void DeleteWindow();
	void OpenFile();

private:
	SDL_Window &m_Window;
	uaudio::AudioSystem &m_AudioSystem;
	uaudio::SoundSystem m_SoundSystem;

	ImguiWindowStruct m_ControlsWindow, m_SoundsWindow, m_ChannelsWindow;
};
