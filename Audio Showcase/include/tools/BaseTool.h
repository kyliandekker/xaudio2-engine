#pragma once

#include <string>

#include <imgui/imgui.h>

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
#define SETTINGS "\xef\x80\x93"
#define MOON "\xef\x86\x86"
#define SUN "\xef\x86\x85"
#define RIGHT "\xef\x81\xa1"
#define LEFT "\xef\x81\xa0"
#define SAVE "\xef\x83\x87"

#define IMGUI_INDENT 16.0f

class BaseTool
{
public:
	BaseTool(ImGuiWindowFlags a_Flags, std::string a_Name, std::string a_Category, bool a_FullScreen = false);
	~BaseTool();

	static float GetRGBColor(int color);
	void ShowValue(const char* a_Text, const char* a_Value);
	virtual void WindowBegin();
	virtual void WindowEnd();
	void Update();

	void SetSize(ImVec2 a_Size);
	bool IsFullScreen() const;

	void SetEnabled(bool a_Enabled);
	void ToggleEnabled();
	std::string GetName() const;
	bool IsEnabled() const;
	std::string GetCategory() const;
protected:
	virtual void Render() = 0;

	ImGuiWindowFlags m_Flags = 0;
	ImVec2 m_Size = ImVec2(0, 0);
	std::string m_Name;
	std::string m_Category;
	bool m_FullScreen = false;
	bool m_Enabled = true;
};