#pragma once

#include <sdl/SDL.h>

#include "AudioImGuiWindow.h"
#include <uaudio/system/high_level/AudioSystem.h>
#include <uaudio/system/high_level/SoundSystem.h>

class AudioSDLWindow
{
public:
	AudioSDLWindow(uaudio::AudioSystem& a_AudioSystem);
	~AudioSDLWindow();

	void RenderWindow();

	AudioImGuiWindow& GetImGuiWindow() const;

	ImVec2 GetWindowSize() const;
private:
	AudioImGuiWindow* m_AudioWindow = nullptr;
	uaudio::AudioSystem& m_AudioSystem;

	int32_t CreateSDLWindow();
	int32_t CreateContext();
	int32_t CreateGlad();
	int32_t CreateImGui();

	SDL_Window *m_Window = nullptr;
	SDL_GLContext m_glContext = nullptr;

	bool m_Running = true;
	int32_t windowWidth = 1280, windowHeight = 720;
	const char *m_WindowTitle = "XAudio2 Showcase";
};