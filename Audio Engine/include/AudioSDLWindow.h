#pragma once

#include <sdl/SDL.h>
#include <string>

#include "AudioImGuiWindow.h"
#include "Audio/AudioSystem.h"

class AudioSDLWindow
{
public:
	AudioSDLWindow();
	~AudioSDLWindow();
	void RenderWindow();
private:
	int CreateWindow();

	SDL_Window* m_Window = nullptr;
	AudioSystem m_AudioSystem;
	AudioImGuiWindow* m_AudioWindow = nullptr;
	bool m_Running = true;
	int windowWidth = 1280, windowHeight = 720;
	SDL_GLContext m_GLContext = nullptr;
	const char* m_WindowTitle = "XAudio2 Showcase";
};