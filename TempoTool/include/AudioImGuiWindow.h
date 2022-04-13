#pragma once

#include <imgui/backends/imgui_impl_sdl.h>
#include <sdl/SDL_video.h>

#include <ImguiWindowStruct.h>

#include "ImGuiAudioChannel.h"
#include "ImGuiWaveFile.h"
#include "xaudio2_engine/AudioSystem.h"

class AudioImGuiWindow
{
public:
	AudioImGuiWindow(SDL_Window &a_Window);
	uint32_t CreateImGui(const SDL_GLContext &a_Context, const char *a_Glslversion) const;
	void RenderImGui();
	void ShowValue(const char *a_Text, const char *a_Value);
	void DeleteWindow();
	void OpenFile();
	void Update();

private:
	std::vector<ImGuiWaveFile> m_Sounds = std::vector<ImGuiWaveFile>();
	std::vector<ImGuiAudioChannel> m_Channels = std::vector<ImGuiAudioChannel>();
	SDL_Window &m_Window;

	ImguiWindowStruct m_ControlsWindow, m_SoundsWindow;
	AudioSystem m_AudioSystem;
};
