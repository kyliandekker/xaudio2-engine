﻿#include "AudioImGuiWindow.h"

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

        if (ImGui::Button("Add Sound"))
            OpenFile();

        for (uint32_t i = 0; i < m_Sounds.size(); i++)
        {
            WaveFile* sound = m_Sounds[i];

            if (sound == nullptr)
                continue;

            if (ImGui::CollapsingHeader(sound->GetSoundTitle()))
            {
                ImGui::Indent(16.0f);

                if (ImGui::CollapsingHeader(std::string("File Info###FileInfo_" + std::to_string(i)).c_str()))
                {
                    ImGui::Indent(16.0f);

                    ShowValue("Name: ", sound->GetSoundTitle());
                    ShowValue("Format: ", std::to_string(sound->GetWavFormat().fmtChunk.audioFormat).c_str());
                    ShowValue("Number Of Channels: ", std::string(std::to_string(sound->GetWavFormat().fmtChunk.numChannels) + (sound->GetWavFormat().fmtChunk.numChannels == 1 ? " (mono)" : " (stereo)")).c_str());
                    ShowValue("Sample Rate: ", std::to_string(sound->GetWavFormat().fmtChunk.sampleRate).c_str());
                    ShowValue("Byte Rate: ", std::string(std::to_string(sound->GetWavFormat().fmtChunk.byteRate) + " Hz").c_str());
                    ShowValue("Bits Per Sample: ", std::to_string(sound->GetWavFormat().fmtChunk.bitsPerSample).c_str());
                    ShowValue("Duration (sec): ", std::string(std::to_string(sound->GetDuration()) + " secs").c_str());
                    if (strcmp(std::string(&sound->GetWavFormat().bextChunk.origination_date[0], &sound->GetWavFormat().bextChunk.origination_date[0] + std::size(sound->GetWavFormat().bextChunk.origination_date)).c_str(), "") != 0)
                        ShowValue("Origination Date: ", std::string(&sound->GetWavFormat().bextChunk.origination_date[0], &sound->GetWavFormat().bextChunk.origination_date[0] + std::size(sound->GetWavFormat().bextChunk.origination_date)).c_str());
                    if (strcmp(std::string(&sound->GetWavFormat().bextChunk.origination_time[0], &sound->GetWavFormat().bextChunk.origination_time[0] + std::size(sound->GetWavFormat().bextChunk.origination_time)).c_str(), "") != 0)
                        ShowValue("Origination Time: ", std::string(&sound->GetWavFormat().bextChunk.origination_time[0], &sound->GetWavFormat().bextChunk.origination_time[0] + std::size(sound->GetWavFormat().bextChunk.origination_time)).c_str());
                    ShowValue("Duration: ", WaveFile::FormatDuration(sound->GetDuration()).c_str());
                    if (sound->GetWavFormat().filledAcidChunk)
                        ShowValue("Tempo: ", std::to_string(sound->GetWavFormat().acidChunk.tempo).c_str());
                    else
                    {
                        const ImVec4 color = ImVec4(GetRGBColor(255), GetRGBColor(0), GetRGBColor(0), 1.0f);
                        ImGui::TextColored(color, "%s\n", "WARNING: NO TEMPO INFO FOUND!");

                        static float tempo;
                        ImGui::InputFloat("Tempo", &tempo, 10.0f, 9000.0f);
                        if (ImGui::Button("Add tempo info"))
                            sound->AddAcidChunk(tempo);
                    }
                    if (ImGui::Button("Remove sound"))
                        m_Sounds.erase(m_Sounds.begin() + i);

                    ImGui::Unindent(16.0f);
                }
                ImGui::Unindent(16.0f);
            }
        }
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

void AudioImGuiWindow::OpenFile()
{
    OPENFILENAME ofn;
    TCHAR sz_file[260] = {0};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = sz_file;
    ofn.nMaxFile = sizeof(sz_file);
    ofn.lpstrFilter = L"WAV Files (*.wav;*.wave)\0*.wav;*.wave";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn))
    {
        char *path = new char[wcslen(ofn.lpstrFile) + 1];
        wsprintfA(path, "%S", ofn.lpstrFile);
        m_Sounds.push_back(new WaveFile(path));
        delete[] path;
    }
}