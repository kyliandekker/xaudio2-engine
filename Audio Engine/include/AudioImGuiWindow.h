﻿#pragma once

#include <imgui/backends/imgui_impl_sdl.h>
#include <sdl/SDL_video.h>

#include "ImguiWindowStruct.h"
#include "Audio/AudioSystem.h"

class AudioImGuiWindow
{
public:
	AudioImGuiWindow();
	AudioImGuiWindow(SDL_Window* a_Window, AudioSystem* a_AudioSystem);
	int CreateImGui(const SDL_GLContext& a_Context, const char* a_Glslversion);
	void EndImGui();
	void RenderImGui();
	void ShowValue(const char* a_Text, const char* a_Value);
	void DeleteWindow();
	void OpenFile();
private:
	SDL_Window* m_Window = nullptr;
	AudioSystem* m_AudioSystem = nullptr;

	ImguiWindowStruct m_ControlsWindow, m_SoundsWindow, m_ChannelsWindow;
};