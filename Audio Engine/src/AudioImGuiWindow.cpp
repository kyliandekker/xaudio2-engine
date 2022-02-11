#include "AudioImGuiWindow.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <sdl/SDL_cpuinfo.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <imgui/implot.h>
#include <imgui/imgui_plot.h>

#include <imgui/backends/imgui_impl_sdl.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "Audio/XAudio2Callback.h"
#include "Audio/XAudio2Channel.h"

AudioImGuiWindow::AudioImGuiWindow(SDL_Window& a_Window, AudioSystem& a_AudioSystem) : m_Window(a_Window), m_AudioSystem(a_AudioSystem)
{
}

int AudioImGuiWindow::CreateImGui(const SDL_GLContext &a_Context, const char *a_Glslversion)
{
    // setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // setup Dear ImGui style
    ImGui::StyleColorsDark();

    // setup platform/renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(&m_Window, a_Context);
    ImGui_ImplOpenGL3_Init(a_Glslversion);

    return 0;
}

void AudioImGuiWindow::RenderImGui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(&m_Window);
    ImGui::NewFrame();

    {
        int sdl_width = 0, sdl_height = 0;
        SDL_GetWindowSize(&m_Window, &sdl_width, &sdl_height);
        int controls_width = sdl_width;

        controls_width -= 10;
        controls_width /= 3;

        m_ControlsWindow.m_Pos = ImVec2(10, 10);

        ImGui::SetNextWindowPos(m_ControlsWindow.m_Pos, ImGuiCond_Always);

        const int new_height = (sdl_height - 30) / 3;

        m_ControlsWindow.m_Size = ImVec2(static_cast<float>(controls_width), static_cast<float>(new_height));

        ImGui::SetNextWindowSize(
            m_ControlsWindow.m_Size,
            ImGuiCond_Always);

        ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoResize);

        ImGui::Dummy(ImVec2(0.0f, 1.0f));
        if (ImGui::CollapsingHeader("Master Actions###MasterActions_0"))
        {
            ImGui::Indent(16.0f);

            ImGui::Text("Master Panning");
            ImGui::SameLine();
            float panning = m_AudioSystem.GetPlayer().GetPanning();
            ImGui::SliderFloat("###Panning_Master_0", &panning, -1, 1);
            m_AudioSystem.GetPlayer().SetPanning(panning);

            ImGui::Text("Master Volume");
            ImGui::SameLine();
            float volume = m_AudioSystem.GetPlayer().GetVolume();
            ImGui::SliderFloat("###Volume_Master_0", &volume, 0, 1);
            m_AudioSystem.GetPlayer().SetVolume(volume);

            if (m_AudioSystem.IsPlaying())
            {
                if (ImGui::Button("Pause Whole Playback"))
                    m_AudioSystem.Pause();
            }
            else
            {
                if (ImGui::Button("Resume Whole Playback"))
                    m_AudioSystem.Resume();
            }
            if (ImGui::Button("Add Sound"))
            {
                OpenFile();
            }
        }

        ImGui::End();
    }

    {
        m_SoundsWindow.m_Pos = ImVec2(10, 20 + m_ControlsWindow.m_Size.y);

        ImGui::SetNextWindowPos(m_SoundsWindow.m_Pos, ImGuiCond_Always);

        m_SoundsWindow.m_Size = ImVec2(m_ControlsWindow.m_Size.x, m_ControlsWindow.m_Size.y);

        ImGui::SetNextWindowSize(
            m_SoundsWindow.m_Size,
            ImGuiCond_Always);

        ImGui::Begin("Sounds", nullptr, ImGuiWindowFlags_NoResize);

        ImGui::Dummy(ImVec2(0.0f, 1.0f));

        const std::vector<WaveFile *> &sounds = m_AudioSystem.GetSounds();
        for (int i = 0; i < static_cast<int>(sounds.size()); i++)
        {
            WaveFile &sound = *sounds[i];
            if (ImGui::CollapsingHeader(sound.GetSoundTitle()))
            {
                ImGui::Indent(16.0f);

                if (ImGui::CollapsingHeader(std::string("File Info###FileInfo_" + std::to_string(i)).c_str()))
                {
                    ImGui::Indent(16.0f);

                    ShowValue("Name: ", sound.GetSoundTitle());
                    ShowValue("Format: ", std::to_string(sound.GetWavFormat().audioFormat).c_str());
                    ShowValue("Number Of Channels: ", std::string(std::to_string(sound.GetWavFormat().numChannels) + (sound.GetWavFormat().numChannels == 1 ? " (mono)" : " (stereo)")).c_str());
                    ShowValue("Sample Rate: ", std::to_string(sound.GetWavFormat().sampleRate).c_str());
                    ShowValue("Byte Rate: ", std::string(std::to_string(sound.GetWavFormat().byteRate) + " Hz").c_str());
                    ShowValue("Bits Per Sample: ", std::to_string(sound.GetWavFormat().bitsPerSample).c_str());
                    ShowValue("Duration (sec): ", std::string(std::to_string(sound.GetDuration()) + " secs").c_str());
                    ShowValue("Duration: ", WaveFile::FormatDuration(sound.GetDuration()).c_str());

                    ImGui::Unindent(16.0f);
                }
                if (ImGui::CollapsingHeader(std::string("File Actions###FileActions_" + std::to_string(i)).c_str()))
                {
                    ImGui::Indent(16.0f);

                    bool isLooping = sound.IsLooping();
                    ImGui::Checkbox("Loop", &isLooping);
                    sound.SetLooping(isLooping);

                    ImGui::Text("Volume");
                    ImGui::SameLine();
                    float volume = sound.GetVolume();
                    ImGui::SliderFloat(std::string("###Volume_Sound_" + std::to_string(i)).c_str(), &volume, 0, 1);
                    sound.SetVolume(volume);

                    if (ImGui::Button(std::string("Play###Play_Sound_" + std::to_string(i)).c_str()))
                    {
                        m_AudioSystem.Play(i);
                    }
	                if (ImGui::Button(std::string("Remove###Remove_Sound_" + std::to_string(i)).c_str()))
	                {
	                    m_AudioSystem.RemoveSound(i);
	                }
	                if (ImGui::Button(std::string("Pause All Channels With This Sound###PauseAllChannelsWith_Sound_" + std::to_string(i)).c_str()))
	                {
	                    m_AudioSystem.PauseAllChannelsWithSound(i);
	                }
	                if (ImGui::Button(std::string("Resume All Channels With This Sound###ResumeAllChannelsWith_Sound_" + std::to_string(i)).c_str()))
	                {
	                    m_AudioSystem.ResumeAllChannelsWithSound(i);
	                }

                    ImGui::Unindent(16.0f);
	            }
                ImGui::Unindent(16.0f);
            }
        }

        ImGui::End();
    }

    {
        m_ChannelsWindow.m_Pos = ImVec2(10, 30 + m_SoundsWindow.m_Size.y + m_ControlsWindow.m_Size.y);

        ImGui::SetNextWindowPos(m_ChannelsWindow.m_Pos, ImGuiCond_Always);

        m_ChannelsWindow.m_Size = ImVec2(m_SoundsWindow.m_Size.x, m_SoundsWindow.m_Size.y - 10);

        ImGui::SetNextWindowSize(
            m_ChannelsWindow.m_Size,
            ImGuiCond_Always);

        ImGui::Begin("Channels", nullptr, ImGuiWindowFlags_NoResize);

        ImGui::Dummy(ImVec2(0.0f, 1.0f));

        const std::vector<BaseChannel *> &channels = m_AudioSystem.GetChannels();
        for (int i = 0; i < static_cast<int>(channels.size()); i++)
        {
            BaseChannel &channel = *channels[i];
            if (ImGui::CollapsingHeader(std::string("Channel_" + std::to_string(i)).c_str()))
            {
                ImGui::Indent(16.0f);

                ShowValue("Is in use: ", channel.IsInUse() ? "true" : "false");

                if (channel.IsInUse())
                {
                    if (ImGui::CollapsingHeader(std::string("Channel Info###ChannelInfo_" + std::to_string(i)).c_str()))
                    {
                        ImGui::Indent(16.0f);

                        ShowValue("Currently playing: ", channel.GetSound().GetSoundTitle());
                        ShowValue("Progress", std::string(
                            WaveFile::FormatDuration(channel.GetCurrentDataPos() / channel.GetSound().GetWavFormat().byteRate) +
                            "/" +
                            WaveFile::FormatDuration(channel.GetSound().GetDuration()))
                            .c_str()
                        );
                        ShowValue("Time Left", std::string(
                            WaveFile::FormatDuration(channel.GetSound().GetDuration() - (channel.GetCurrentDataPos() / channel.GetSound().GetWavFormat().byteRate))
                        ).c_str());

                        ImGui::Unindent(16.0f);
                    }

                    if (ImGui::CollapsingHeader(std::string("Channel Actions###ChannelActions_" + std::to_string(i)).c_str()))
                    {
                        ImGui::Indent(16.0f);

                        ImGui::Text("Panning");
                        ImGui::SameLine();
                        float panning = channel.GetPanning();
                        ImGui::SliderFloat(std::string("###Panning_Channel_" + std::to_string(i)).c_str(), &panning, -1, 1);
                        channel.SetPanning(panning);

                        ImGui::Text("Volume");
                        ImGui::SameLine();
                        float volume = channel.GetVolume();
                        ImGui::SliderFloat(std::string("###Volume_Channel_" + std::to_string(i)).c_str(), &volume, 0, 1);
                        channel.SetVolume(volume);

                        if (channel.IsPlaying())
                        {
                            if (ImGui::Button(std::string("Pause###Pause_Channel_" + std::to_string(i)).c_str()))
                                channel.Pause();
                        }
                        else
                        {
                            if (ImGui::Button(std::string("Resume###Resume_Channel_" + std::to_string(i)).c_str()))
                                channel.Resume();
                        }

                        ImGui::Unindent(16.0f);
                    }
                }

                ImGui::Unindent(16.0f);
            }
        }

        {
            int sdl_width = 0, sdl_height = 0;
            SDL_GetWindowSize(&m_Window, &sdl_width, &sdl_height);
            int controls_width = sdl_width;

            controls_width -= 10;
            controls_width /= 3;

            m_ControlsWindow.m_Pos = ImVec2((controls_width * 2), 10);

            ImGui::SetNextWindowPos(m_ControlsWindow.m_Pos, ImGuiCond_Always);

            const int new_height = sdl_height - 20;

            m_ControlsWindow.m_Size = ImVec2(static_cast<float>(controls_width), static_cast<float>(new_height));

            ImGui::SetNextWindowSize(
                m_ControlsWindow.m_Size,
                ImGuiCond_Always);
        }

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void AudioImGuiWindow::ShowValue(const char *a_Text, const char *a_Value)
{
    ImGui::Text(a_Text);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), a_Value);
}

void AudioImGuiWindow::DeleteWindow()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

void AudioImGuiWindow::OpenFile()
{
    OPENFILENAME ofn;
    TCHAR szFile[260] = {0};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"WAV Files (*.wav;*.wave)\0*.wav;*.wave";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == true)
    {
        char *path = new char[wcslen(ofn.lpstrFile) + 1];
        wsprintfA(path, "%S", ofn.lpstrFile);
        m_AudioSystem.CreateSound(path);
        delete[] path;
    }
}