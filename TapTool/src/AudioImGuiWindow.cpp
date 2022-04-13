#include "AudioImGuiWindow.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <sdl/SDL_cpuinfo.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <imgui/backends/imgui_impl_sdl.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <xaudio2_engine/xaudio2/XAudio2Callback.h>
#include <xaudio2_engine/xaudio2/XAudio2Channel.h>

AudioImGuiWindow::AudioImGuiWindow(SDL_Window &a_Window) : m_Window(a_Window)
{
}

uint32_t AudioImGuiWindow::CreateImGui(const SDL_GLContext &a_Context, const char *a_Glslversion) const
{
    // setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    const ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // setup Dear ImGui style
    ImGui::StyleColorsDark();

    // setup platform/renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(&m_Window, a_Context);
    ImGui_ImplOpenGL3_Init(a_Glslversion);

    return 0;
}

float GetRGBColor(int color)
{
    return 1.0f / 255.0f * static_cast<float>(color);
}

void AudioImGuiWindow::RenderImGui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(&m_Window);
    ImGui::NewFrame();

    {
        int sdl_width = 0, sdl_height = 0;
        SDL_GetWindowSize(&m_Window, &sdl_width, &sdl_height);

        ImGui::SetNextWindowPos(m_ControlsWindow.m_Pos, ImGuiCond_Always);

        m_ControlsWindow.m_Size = ImVec2(static_cast<float>(sdl_width), static_cast<float>(sdl_height));

        ImGui::SetNextWindowSize(
            m_ControlsWindow.m_Size,
            ImGuiCond_Always);

        ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoResize);

        ImGui::Dummy(ImVec2(0.0f, 1.0f));
        ImGui::Indent(16.0f);

        if (ImGui::Button("Tap"))
            Tap();
        ShowValue("Calculated BPM", std::to_string(m_CalculatedTempo).c_str());
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void AudioImGuiWindow::ShowValue(const char *a_Text, const char *a_Value)
{
	const ImVec4 color = ImVec4(GetRGBColor(61), GetRGBColor(133), GetRGBColor(224), 1.0f);
    ImGui::Text("%s\n", a_Text);
    ImGui::SameLine();
    ImGui::TextColored(color, "%s\n", a_Value);
}

void AudioImGuiWindow::DeleteWindow()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void AudioImGuiWindow::Calc()
{
    float m = 0.0f;
    for (int  i = 0; i < static_cast<int>(m_Times.size()) - 1; i++)
    {
	    const float s = std::chrono::duration_cast<std::chrono::milliseconds>(m_Times[i + 1] - m_Times[i]).count();
        m += s;
    }
    m /= static_cast<float>(m_Times.size() - 1);
    m = 60000.0f / m;
    m_CalculatedTempo = static_cast<unsigned int>(m);
    if (m_Times.size() > 5)
		m_Times.erase(m_Times.begin(), m_Times.size() > 1 ? m_Times.begin() + 1 : m_Times.end());
}

void AudioImGuiWindow::Tap()
{
    m_Times.push_back(std::chrono::high_resolution_clock::now());
    Calc();
}

void AudioImGuiWindow::Update()
{
}
