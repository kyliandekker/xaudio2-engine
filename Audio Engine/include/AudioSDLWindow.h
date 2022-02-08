#pragma once

#include <glad/glad.h>
#include <sdl/SDL.h>
#include <string>
#include <thread>

#include "AudioImGuiWindow.h"
#include "Audio/AudioSystem.h"

class AudioSDLWindow
{
public:
	AudioSDLWindow();
	int CreateWindow();
	void RenderWindow();
	void DeleteWindow();
	void UpdateAudio();
private:
	SDL_Window* m_Window = nullptr;
	AudioSystem m_AudioSystem;
	AudioImGuiWindow m_AudioWindow = AudioImGuiWindow();
	bool m_Running = true;
	int windowWidth = 1280, windowHeight = 720;
	std::string m_GLSLversion = "";
	SDL_GLContext m_GLContext = nullptr;
	const char* m_WindowTitle = "XAudio2 Showcase";
};
