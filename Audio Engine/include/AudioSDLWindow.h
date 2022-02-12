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

	uint32_t CreateSDLWindow();
	uint32_t CreateContext();
	uint32_t CreateGlad();
	uint32_t CreateImGui();

	SDL_Window *m_Window = nullptr;
	SDL_GLContext m_glContext = nullptr;

	bool m_Running = true;
	uint32_t windowWidth = 1280, windowHeight = 720;
	const char *m_WindowTitle = "XAudio2 Showcase";
};