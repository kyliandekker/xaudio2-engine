#pragma once

#include <imgui/backends/imgui_impl_sdl.h>
#include <sdl/SDL_video.h>

#include <ImguiWindowStruct.h>

#include "xaudio2_engine/wav/WaveFile.h"

class AudioImGuiWindow
{
public:
	AudioImGuiWindow(SDL_Window &a_Window);
	uint32_t CreateImGui(const SDL_GLContext &a_Context, const char *a_Glslversion) const;
	void RenderImGui();
	void ShowValue(const char *a_Text, const char *a_Value);
	void DeleteWindow();
	void OpenFile();

private:
	std::vector<WaveFile*> m_Sounds = std::vector<WaveFile*>();
	SDL_Window &m_Window;

	ImguiWindowStruct m_ControlsWindow, m_SoundsWindow;
};
