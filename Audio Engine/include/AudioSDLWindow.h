#pragma once

#include <sdl/SDL.h>

#include "AudioImGuiWindow.h"
#include "Audio/AudioSystem.h"

class AudioSDLWindow
{
public:
	AudioSDLWindow();
	~AudioSDLWindow();

	void RenderWindow();
private:
	AudioImGuiWindow* m_AudioWindow = nullptr;
	AudioSystem m_AudioSystem;

	int CreateWindow();
	int CreateContext();
	int CreateGlad();
	int CreateImGui();

	SDL_Window* m_Window = nullptr;
	SDL_GLContext m_glContext = nullptr;

	bool m_Running = true;
	int windowWidth = 1280, windowHeight = 720;
	const char* m_WindowTitle = "XAudio2 Showcase";
};