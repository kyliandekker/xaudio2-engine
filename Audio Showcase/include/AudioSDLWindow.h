#pragma once

#include <sdl/SDL.h>

#include "AudioImGuiWindow.h"
#include <AudioSystem.h>

class AudioSDLWindow
{
public:
	AudioSDLWindow(uaudio::AudioSystem* a_AudioSystem, uaudio::SoundSystem* a_SoundSystem);
	~AudioSDLWindow();

	void RenderWindow();

	ImVec2 GetWindowSize() const;
private:
	AudioImGuiWindow m_AudioWindow;
	uaudio::AudioSystem* m_AudioSystem;
	uaudio::SoundSystem* m_SoundSystem;

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