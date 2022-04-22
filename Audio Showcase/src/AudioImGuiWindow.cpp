#include "AudioImGuiWindow.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <sdl/SDL_cpuinfo.h>

#include "AudioSDLWindow.h"

#include <imgui/imgui.h>

#include <uaudio/xaudio2/XAudio2Callback.h>

#include "tools/MainWindow.h"

AudioImGuiWindow::AudioImGuiWindow() = default;

AudioImGuiWindow::AudioImGuiWindow(AudioSDLWindow* a_Window, uaudio::AudioSystem* a_AudioSystem, uaudio::SoundSystem* a_SoundSystem) : m_Window(a_Window), m_AudioSystem(a_AudioSystem), m_SoundSystem(a_SoundSystem)
{
    m_MainWindow = new MainWindow(m_Tools);
}

AudioImGuiWindow::~AudioImGuiWindow()
{
    m_Enabled = false;

    delete m_MainWindow;

    m_Tools.clear();
}

float GetRGBColor(int color)
{
    return 1.0f / 255.0f * static_cast<float>(color);
}

ImVec4 ColorFromBytes(int r, int g, int b)
{
    return ImVec4(GetRGBColor(r), GetRGBColor(g), GetRGBColor(b), 1);
}

ImFont* m_DefaultFont = nullptr;
ImFont* m_EditorFont = nullptr;

/// <summary>
/// Creates a context and sets everything up for ImGui.
/// </summary>
void AudioImGuiWindow::CreateImGui() const
{
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    (void)io;

    // setup Dear ImGui style
    ImGui::StyleColorsDark();

    ImFont* font1 = io.Fonts->AddFontDefault();
    (void)font1;

    m_DefaultFont = io.Fonts->AddFontFromFileTTF("./resources/font_default.ttf", 14.0f);

	constexpr auto ICON_MIN_FA = 0xf000;
	constexpr auto ICON_MAX_FA = 0xf2e0;
	constexpr ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    m_EditorFont = io.Fonts->AddFontFromFileTTF("./resources/fontawesome.ttf", 14.0f, &icons_config, icons_ranges);
    io.Fonts->Build();
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();

    ImGuiStyle m_DarkStyle = ImGui::GetStyle(); const ImVec4 bgColor = ColorFromBytes(37, 37, 38);
    const ImVec4 lightBgColor = ColorFromBytes(82, 82, 85);
    const ImVec4 veryLightBgColor = ColorFromBytes(90, 90, 95);

    const ImVec4 panelColor = ColorFromBytes(51, 51, 55);
    const ImVec4 panelHoverColor = ColorFromBytes(70, 70, 70);
    const ImVec4 panelHoverColor2 = ColorFromBytes(80, 80, 80);
    const ImVec4 panelDark = ColorFromBytes(29, 170, 200);
    const ImVec4 panelActiveColor = ColorFromBytes(0, 119, 200);
    const ImVec4 panelActiverColor = ColorFromBytes(0, 119, 150);

    const ImVec4 textColor = ColorFromBytes(255, 255, 255);
    const ImVec4 textDisabledColor = ColorFromBytes(151, 151, 151);
    const ImVec4 borderColor = ColorFromBytes(78, 78, 78);

    m_DarkStyle.Colors[ImGuiCol_Text] = textColor;
    m_DarkStyle.Colors[ImGuiCol_TextDisabled] = textDisabledColor;
    m_DarkStyle.Colors[ImGuiCol_TextSelectedBg] = panelActiveColor;
    m_DarkStyle.Colors[ImGuiCol_WindowBg] = bgColor;
    m_DarkStyle.Colors[ImGuiCol_ChildBg] = bgColor;
    m_DarkStyle.Colors[ImGuiCol_PopupBg] = bgColor;
    m_DarkStyle.Colors[ImGuiCol_Border] = borderColor;
    m_DarkStyle.Colors[ImGuiCol_BorderShadow] = borderColor;
    m_DarkStyle.Colors[ImGuiCol_FrameBg] = panelColor;
    m_DarkStyle.Colors[ImGuiCol_FrameBgHovered] = panelHoverColor;
    m_DarkStyle.Colors[ImGuiCol_FrameBgActive] = panelHoverColor2;
    m_DarkStyle.Colors[ImGuiCol_TitleBg] = bgColor;
    m_DarkStyle.Colors[ImGuiCol_TitleBgActive] = bgColor;
    m_DarkStyle.Colors[ImGuiCol_TitleBgCollapsed] = bgColor;
    m_DarkStyle.Colors[ImGuiCol_MenuBarBg] = panelColor;
    m_DarkStyle.Colors[ImGuiCol_ScrollbarBg] = panelColor;
    m_DarkStyle.Colors[ImGuiCol_ScrollbarGrab] = lightBgColor;
    m_DarkStyle.Colors[ImGuiCol_ScrollbarGrabHovered] = veryLightBgColor;
    m_DarkStyle.Colors[ImGuiCol_ScrollbarGrabActive] = veryLightBgColor;
    m_DarkStyle.Colors[ImGuiCol_CheckMark] = panelActiveColor;
    m_DarkStyle.Colors[ImGuiCol_SliderGrab] = panelDark;
    m_DarkStyle.Colors[ImGuiCol_SliderGrabActive] = panelActiveColor;
    m_DarkStyle.Colors[ImGuiCol_Button] = panelDark;
    m_DarkStyle.Colors[ImGuiCol_ButtonHovered] = panelActiveColor;
    m_DarkStyle.Colors[ImGuiCol_ButtonActive] = panelActiverColor;
    m_DarkStyle.Colors[ImGuiCol_Header] = panelColor;
    m_DarkStyle.Colors[ImGuiCol_HeaderHovered] = panelHoverColor;
    m_DarkStyle.Colors[ImGuiCol_HeaderActive] = panelActiveColor;
    m_DarkStyle.Colors[ImGuiCol_Separator] = borderColor;
    m_DarkStyle.Colors[ImGuiCol_SeparatorHovered] = borderColor;
    m_DarkStyle.Colors[ImGuiCol_SeparatorActive] = borderColor;
    m_DarkStyle.Colors[ImGuiCol_ResizeGrip] = bgColor;
    m_DarkStyle.Colors[ImGuiCol_ResizeGripHovered] = panelColor;
    m_DarkStyle.Colors[ImGuiCol_ResizeGripActive] = lightBgColor;
    m_DarkStyle.Colors[ImGuiCol_PlotLines] = panelActiveColor;
    m_DarkStyle.Colors[ImGuiCol_PlotLinesHovered] = panelHoverColor;
    m_DarkStyle.Colors[ImGuiCol_PlotHistogram] = panelActiveColor;
    m_DarkStyle.Colors[ImGuiCol_PlotHistogramHovered] = panelHoverColor;
    m_DarkStyle.Colors[ImGuiCol_DragDropTarget] = bgColor;
    m_DarkStyle.Colors[ImGuiCol_NavHighlight] = bgColor;
    m_DarkStyle.Colors[ImGuiCol_DockingPreview] = panelActiveColor;
    m_DarkStyle.Colors[ImGuiCol_Tab] = bgColor;
    m_DarkStyle.Colors[ImGuiCol_TabActive] = panelActiveColor;
    m_DarkStyle.Colors[ImGuiCol_TabUnfocused] = bgColor;
    m_DarkStyle.Colors[ImGuiCol_TabUnfocusedActive] = panelActiveColor;
    m_DarkStyle.Colors[ImGuiCol_TabHovered] = panelHoverColor;

    m_DarkStyle.WindowPadding = ImVec2(10, 10);
    m_DarkStyle.WindowRounding = 10.0f;
    m_DarkStyle.FramePadding = ImVec2(5, 5);
    m_DarkStyle.ItemSpacing = ImVec2(12, 8);
    m_DarkStyle.ItemInnerSpacing = ImVec2(8, 6);
    m_DarkStyle.IndentSpacing = 25.0f;
    m_DarkStyle.ScrollbarSize = 15.0f;
    m_DarkStyle.ScrollbarRounding = 9.0f;
    m_DarkStyle.GrabMinSize = 20.0f; // Make grab a circle
    m_DarkStyle.GrabRounding = 12.0f;
    m_DarkStyle.PopupRounding = 7.f;
    m_DarkStyle.Alpha = 1.0;

    style = m_DarkStyle;
}

/// <summary>
/// Renders the ImGui windows.
/// </summary>
void AudioImGuiWindow::Render()
{
    if (!m_Enabled)
        return;

    ImGui::NewFrame();
    
	ImGui::PushFont(m_DefaultFont);

    m_MainWindow->SetSize(m_Window->GetWindowSize());
    m_MainWindow->Update();
    for (auto* tool : m_Tools)
    {
        if (tool->IsFullScreen())
            tool->SetSize(m_Window->GetWindowSize());
        tool->Update();
    }

    ImGui::PopFont();

    ImGui::Render();
}

/// <summary>
/// Deletes the SDL window.
/// </summary>
void AudioImGuiWindow::DeleteWindow() const
{
    ImGui::DestroyContext();
}

void AudioImGuiWindow::AddTool(BaseTool* a_Tool)
{
    m_Tools.push_back(a_Tool);
}