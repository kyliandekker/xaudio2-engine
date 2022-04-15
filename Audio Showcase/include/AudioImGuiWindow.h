#pragma once

#include <imgui/backends/imgui_impl_sdl.h>
#include <sdl/SDL_video.h>

#include <ImguiWindowStruct.h>
#include <AudioSystem.h>

#include "SoundSystem.h"

// https://github.com/juliettef/IconFontCppHeaders/blob/main/IconsFontAwesome4.h

#define PLAY "\xef\x81\x8b"
#define PAUSE "\xef\x81\x8c"
#define STOP "\xef\x81\x8d"
#define VOLUME_OFF "\xef\x80\xa6"
#define VOLUME_DOWN "\xef\x80\xa7"
#define VOLUME_UP "\xef\x80\xa8"
#define MUSIC_NOTE "\xef\x80\x81"
#define FONT "\xef\x80\xb1"
#define SPRITE "\xef\x87\x85"
#define CROP "\xef\x84\xa5"
#define CONTROLLER "\xef\x84\x9b"
#define RETRY "\xef\x80\xa1"
#define SAVE "\xef\x83\x87"
#define MINUS "\xef\x81\xa8"
#define ADD "\xef\x81\xa7"
#define PANNING "\xef\x87\x9e"
#define EXCLAMATION "\xef\x84\xaa"
#define CLOCK "\xef\x80\x97"

#define IMGUI_INDENT 16.0f

class AudioSDLWindow;

class AudioImGuiWindow
{
public:
	AudioImGuiWindow();
	AudioImGuiWindow(AudioSDLWindow* a_Window, uaudio::AudioSystem* a_AudioSystem, uaudio::SoundSystem* a_SoundSystem);
	uint32_t CreateImGui() const;
	float GetRGBColor(int color);
	void RenderImGui();
	void ShowValue(const char *a_Text, const char *a_Value);
	void RenderSound(uaudio::WaveFile& a_WaveFile);
	void RenderChannel(uint32_t a_Index, uaudio::xaudio2::XAudio2Channel* a_Channel);
	void DeleteWindow();
	void OpenFile();
private:
	AudioSDLWindow* m_Window = nullptr;

	uaudio::AudioSystem* m_AudioSystem = nullptr;
	uaudio::SoundSystem* m_SoundSystem = nullptr;

	ImguiWindowStruct m_ControlsWindow, m_SoundsWindow, m_ChannelsWindow;
};
