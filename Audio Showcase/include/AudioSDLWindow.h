#pragma once

#include <sdl/SDL.h>

#include "AudioImGuiWindow.h"
#include <AudioSystem.h>

class AudioSDLWindow
{
public:
	AudioSDLWindow();
	~AudioSDLWindow();

	void RenderWindow();

private:
	AudioImGuiWindow *m_AudioWindow = nullptr;
	uaudio::AudioSystem m_AudioSystem;

	int32_t CreateSDLWindow();
	int32_t CreateContext();
	int32_t CreateGlad();
	int32_t CreateImGui();

	float GetRGBColor(int color);
	void ShowValue(const char* a_Text, const char* a_Value);

	void RenderSound(uaudio::WaveFile& a_WaveFile);

	SDL_Window *m_Window = nullptr;
	SDL_GLContext m_glContext = nullptr;

	bool m_Running = true;
	int32_t windowWidth = 1280, windowHeight = 720;
	const char *m_WindowTitle = "XAudio2 Showcase";
};