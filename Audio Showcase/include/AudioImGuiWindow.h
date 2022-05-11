#pragma once

#include <imgui/backends/imgui_impl_sdl.h>
#include <sdl/SDL_video.h>

#include <ImguiWindowStruct.h>

#include "tools/BaseTool.h"
#include "tools/MainWindow.h"

#include <uaudio/system/high_level/AudioSystem.h>

class AudioSDLWindow;

class AudioImGuiWindow
{
public:
	AudioImGuiWindow();
	AudioImGuiWindow(AudioSDLWindow* a_Window, uaudio::AudioSystem* a_AudioSystem);
	~AudioImGuiWindow();

	void CreateImGui() const;
	void Render();
	void DeleteWindow() const;
	void AddTool(BaseTool* a_Tool);
private:
	AudioSDLWindow* m_Window = nullptr;

	uaudio::AudioSystem* m_AudioSystem = nullptr;

	std::vector<BaseTool*> m_Tools;

	ImguiWindowStruct m_ControlsWindow, m_SoundsWindow, m_ChannelsWindow;
	MainWindow* m_MainWindow = nullptr;
	bool m_Enabled = true;
};
