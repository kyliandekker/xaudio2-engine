#pragma once

#include <sdl/SDL.h>

#include "AudioImGuiWindow.h"
#include <xaudio2_engine/AudioSystem.h>

class AudioSDLWindow
{
public:
	AudioSDLWindow();
	~AudioSDLWindow();

	void RenderWindow();

private:
	AudioImGuiWindow *m_AudioWindow = nullptr;

	int32_t CreateSDLWindow();
	int32_t CreateContext();
	int32_t CreateGlad();
	int32_t CreateImGui();

	SDL_Window *m_Window = nullptr;
	SDL_GLContext m_glContext = nullptr;

	bool m_Running = true;
	int32_t windowWidth = 500, windowHeight = 500;
	const char *m_WindowTitle = "Audio Tool";
};