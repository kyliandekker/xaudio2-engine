#pragma once

#include <sdl/SDL.h>

#include "AudioImGuiWindow.h"
#include <Audio/AudioSystem.h>

class AudioSDLWindow
{
public:
	AudioSDLWindow();
	~AudioSDLWindow();

	void RenderWindow();

private:
	AudioImGuiWindow *m_AudioWindow = nullptr;
	AudioSystem m_AudioSystem;

	int32_t CreateSDLWindow();
	int32_t CreateContext();
	int32_t CreateGlad();
	int32_t CreateImGui();

	SDL_Window *m_Window = nullptr;
	SDL_GLContext m_glContext = nullptr;

	bool m_Running = true;
	uint32_t windowWidth = 1280, windowHeight = 720;
	const char *m_WindowTitle = "XAudio2 Showcase";
};