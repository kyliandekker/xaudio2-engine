#include "AudioImGuiWindow.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <sdl/SDL_cpuinfo.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <imgui/backends/imgui_impl_sdl.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <xaudio2/XAudio2Callback.h>

AudioImGuiWindow::AudioImGuiWindow(SDL_Window &a_Window, uaudio::AudioSystem &a_AudioSystem) : m_Window(a_Window), m_AudioSystem(a_AudioSystem)
{
}

/// <summary>
/// Creates a context and sets everything up for ImGui.
/// </summary>
/// <param name="a_Context">The SDL context that will be passed to ImGui.</param>
/// <param name="a_Glslversion">The version of GLSL.</param>
/// <returns>Returns 0 if successful.</returns>
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

/// <summary>
/// Converts colors.
/// </summary>
/// <param name="color"></param>
/// <returns></returns>
float AudioImGuiWindow::GetRGBColor(int color)
{
    return 1.0f / 255.0f * static_cast<float>(color);
}

/// <summary>
/// Simple wrapper function to show a value in ImGui.
/// </summary>
/// <param name="a_Text">The text that will be shown in front of the value.</param>
/// <param name="a_Value">The value that will be shown after the text.</param>
void AudioImGuiWindow::ShowValue(const char* a_Text, const char* a_Value)
{
    const ImVec4 color = ImVec4(GetRGBColor(255), GetRGBColor(255), GetRGBColor(255), 1.0f);
    ImGui::Text("%s\n", a_Text);
    ImGui::SameLine();
    ImGui::TextColored(color, "%s\n", a_Value);
}

/// <summary>
/// Renders the sound.
/// </summary>
/// <param name="a_WaveFile">The wave file that needs to get rendered.</param>
void AudioImGuiWindow::RenderSound(uaudio::WaveFile& a_WaveFile)
{
    if (ImGui::CollapsingHeader(a_WaveFile.GetSoundTitle()))
    {
        ImGui::Indent(IMGUI_INDENT);

        if (ImGui::CollapsingHeader(std::string(std::string("File Info###FileInfo_") + a_WaveFile.GetSoundTitle()).c_str()))
        {
            ImGui::Indent(IMGUI_INDENT);

            if (ImGui::CollapsingHeader(std::string(std::string("Deeper File Info###DeeperFileInfo_") + a_WaveFile.GetSoundTitle()).c_str()))
            {
                ImGui::Indent(IMGUI_INDENT);
                ShowValue("Block Align: ", std::string(std::to_string(a_WaveFile.GetWavFormat().fmtChunk.blockAlign)).c_str());
                ShowValue("Format: ", std::string(std::to_string(a_WaveFile.GetWavFormat().dataChunk.chunkSize)).c_str());
                ImGui::Unindent(16.0f);
            }
            ShowValue("Name: ", a_WaveFile.GetSoundTitle());
            ShowValue("Format: ", std::to_string(a_WaveFile.GetWavFormat().fmtChunk.audioFormat).c_str());
            ShowValue("Number Of Channels: ", std::string(std::to_string(a_WaveFile.GetWavFormat().fmtChunk.numChannels) + (a_WaveFile.GetWavFormat().fmtChunk.numChannels == 1 ? " (mono)" : " (stereo)")).c_str());
            ShowValue("Sample Rate: ", std::to_string(a_WaveFile.GetWavFormat().fmtChunk.sampleRate).c_str());
            ShowValue("Byte Rate: ", std::string(std::to_string(a_WaveFile.GetWavFormat().fmtChunk.byteRate) + " Hz").c_str());
            ShowValue("Bits Per Sample: ", std::to_string(a_WaveFile.GetWavFormat().fmtChunk.bitsPerSample).c_str());
            ShowValue("Duration (sec): ", std::string(std::to_string(uaudio::WaveFile::GetDuration(a_WaveFile.GetWavFormat().dataChunk.chunkSize, a_WaveFile.GetWavFormat().fmtChunk.byteRate)) + " secs").c_str());
            if (strcmp(std::string(&a_WaveFile.GetWavFormat().bextChunk.origination_date[0], &a_WaveFile.GetWavFormat().bextChunk.origination_date[0] + std::size(a_WaveFile.GetWavFormat().bextChunk.origination_date)).c_str(), "") != 0)
                ShowValue("Origination Date: ", std::string(&a_WaveFile.GetWavFormat().bextChunk.origination_date[0], &a_WaveFile.GetWavFormat().bextChunk.origination_date[0] + std::size(a_WaveFile.GetWavFormat().bextChunk.origination_date)).c_str());
            if (strcmp(std::string(&a_WaveFile.GetWavFormat().bextChunk.origination_time[0], &a_WaveFile.GetWavFormat().bextChunk.origination_time[0] + std::size(a_WaveFile.GetWavFormat().bextChunk.origination_time)).c_str(), "") != 0)
                ShowValue("Origination Time: ", std::string(&a_WaveFile.GetWavFormat().bextChunk.origination_time[0], &a_WaveFile.GetWavFormat().bextChunk.origination_time[0] + std::size(a_WaveFile.GetWavFormat().bextChunk.origination_time)).c_str());
            ShowValue("Duration: ", uaudio::WaveFile::FormatDuration(uaudio::WaveFile::GetDuration(a_WaveFile.GetWavFormat().dataChunk.chunkSize, a_WaveFile.GetWavFormat().fmtChunk.byteRate)).c_str());
            if (a_WaveFile.GetWavFormat().filledAcidChunk)
                ShowValue("Tempo: ", std::to_string(a_WaveFile.GetWavFormat().acidChunk.tempo).c_str());
            if (ImGui::Button(std::string(std::string("Play###PlaySound_") + a_WaveFile.GetSoundTitle()).c_str()))
            {
                m_AudioSystem.Play(a_WaveFile);
            }
            if (ImGui::Button(std::string(std::string("Remove Sound###RemoveSound_") + a_WaveFile.GetSoundTitle()).c_str()))
            {
                //m_Sounds.erase(m_Sounds.begin() + i);
                //m_Channels.erase(m_Channels.begin() + i);
            }

            ImGui::Unindent(IMGUI_INDENT);
        }
        ImGui::Unindent(IMGUI_INDENT);
    }
}

/// <summary>
/// Renders the channel.
/// </summary>
/// <param name="a_Channel">The channel that needs to get rendered.</param>
void AudioImGuiWindow::RenderChannel(uint32_t a_Index, uaudio::xaudio2::XAudio2Channel* a_Channel)
{
    if (ImGui::CollapsingHeader(std::string("Channel " + std::to_string(a_Index) + "##Channel_" + std::to_string(a_Index)).c_str()))
    {
        ImGui::Indent(IMGUI_INDENT);
        ShowValue("Is in use: ", a_Channel->IsInUse() ? "true" : "false");

        if (a_Channel->IsInUse())
        {
            if (ImGui::CollapsingHeader(std::string("Channel Info###ChannelInfo_" + std::to_string(a_Index)).c_str()))
            {
                ImGui::Indent(IMGUI_INDENT);

                ShowValue("Currently playing: ", a_Channel->GetSound().GetSoundTitle());
                ShowValue("Progress", std::string(
                    uaudio::WaveFile::FormatDuration(static_cast<float>(a_Channel->GetPos(uaudio::TIMEUNIT::TIMEUNIT_POS)) / static_cast<float>(a_Channel->GetSound().GetWavFormat().fmtChunk.byteRate)) +
                    "/" +
                    uaudio::WaveFile::FormatDuration(uaudio::WaveFile::GetDuration(a_Channel->GetSound().GetWavFormat().dataChunk.chunkSize, a_Channel->GetSound().GetWavFormat().fmtChunk.byteRate)))
                    .c_str());
                ShowValue("Time Left", std::string(
                    uaudio::WaveFile::FormatDuration(uaudio::WaveFile::GetDuration(a_Channel->GetSound().GetWavFormat().dataChunk.chunkSize, a_Channel->GetSound().GetWavFormat().fmtChunk.byteRate) - (static_cast<float>(a_Channel->GetPos(uaudio::TIMEUNIT::TIMEUNIT_POS)) / static_cast<float>(a_Channel->GetSound().GetWavFormat().fmtChunk.byteRate))))
                    .c_str());

                ImGui::Unindent(IMGUI_INDENT);
            }

            if (ImGui::CollapsingHeader(std::string("Channel Actions###ChannelActions_" + std::to_string(a_Index)).c_str()))
            {
                ImGui::Indent(IMGUI_INDENT);

                ImGui::Text("Panning");
                ImGui::SameLine();
                float panning = a_Channel->GetPanning();
                ImGui::SliderFloat(std::string("###Panning_Channel_" + std::to_string(a_Index)).c_str(), &panning, -1, 1);
                a_Channel->SetPanning(panning);

                ImGui::Text("Volume");
                ImGui::SameLine();
                float volume = a_Channel->GetVolume();
                ImGui::SliderFloat(std::string("###Volume_Channel_" + std::to_string(a_Index)).c_str(), &volume, 0, 1);
                a_Channel->SetVolume(volume);

                int32_t pos = static_cast<int32_t>(a_Channel->GetPos(uaudio::TIMEUNIT::TIMEUNIT_POS));
                if (ImGui::SliderInt(std::string("###Player_" + std::to_string(a_Index)).c_str(), &pos, 0.0f, a_Channel->GetSound().GetWavFormat().dataChunk.chunkSize, ""))
                {
                    uint32_t newtest = pos % m_AudioSystem.GetBufferSize();
                    uint32_t finaltest = pos - newtest;
                    a_Channel->SetPos(finaltest);
                    if (!a_Channel->IsPlaying())
						a_Channel->PlayRanged(finaltest, m_AudioSystem.GetBufferSize());
                }
                ShowValue("Current Pos: ", std::string(
                    uaudio::WaveFile::FormatDuration(static_cast<float>(a_Channel->GetPos(uaudio::TIMEUNIT::TIMEUNIT_POS)) / static_cast<float>(a_Channel->GetSound().GetWavFormat().fmtChunk.byteRate)) +
                    "/" +
                    uaudio::WaveFile::FormatDuration(uaudio::WaveFile::GetDuration(a_Channel->GetSound().GetWavFormat().dataChunk.chunkSize, a_Channel->GetSound().GetWavFormat().fmtChunk.byteRate)))
                    .c_str());
                if (a_Channel->IsPlaying())
                {
                    if (ImGui::Button(std::string("Pause###Pause_Channel_" + std::to_string(a_Index)).c_str()))
                        a_Channel->Pause();
                }
                else
                {
                    if (ImGui::Button(std::string("Resume###Resume_Channel_" + std::to_string(a_Index)).c_str()))
                        a_Channel->Play();
                }

                ImGui::Unindent(IMGUI_INDENT);
            }
        }
        ImGui::Unindent(IMGUI_INDENT);
    }
}

/// <summary>
/// Renders the ImGui windows.
/// </summary>
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
            float panning = m_AudioSystem.GetMasterPanning();
            ImGui::SliderFloat("###Panning_Master_0", &panning, -1, 1);
            m_AudioSystem.SetMasterPanning(panning);

            ImGui::Text("Master Volume");
            ImGui::SameLine();
            float volume = m_AudioSystem.GetMasterVolume();
            ImGui::SliderFloat("###Volume_Master_0", &volume, 0, 1);
            m_AudioSystem.SetMasterVolume(volume);

            if (m_AudioSystem.IsActive())
            {
                if (ImGui::Button("Pause Whole Playback"))
                    m_AudioSystem.SetActive(false);
            }
            else
            {
                if (ImGui::Button("Resume Whole Playback"))
                    m_AudioSystem.SetActive(true);
            }
            if (ImGui::Button("Add Sound"))
                OpenFile();
        }

        ImGui::End();
    }

    {
        m_SoundsWindow.m_Pos = ImVec2(10, 20 + m_ControlsWindow.m_Size.y);

        ImGui::SetNextWindowPos(m_SoundsWindow.m_Pos, ImGuiCond_Always);

        m_SoundsWindow.m_Size = ImVec2(m_ControlsWindow.m_Size.x, m_ControlsWindow.m_Size.y);

        ImGui::SetNextWindowSize(m_SoundsWindow.m_Size, ImGuiCond_Always);

        ImGui::Begin("Sounds", nullptr, ImGuiWindowFlags_NoResize);

        ImGui::Dummy(ImVec2(0.0f, 1.0f));

        for (auto* sound : m_SoundSystem.GetSounds())
            RenderSound(*sound);

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

        for (uint32_t i = 0; i < m_AudioSystem.ChannelSize(); i++)
            RenderChannel(i, m_AudioSystem.GetChannel(uaudio::ChannelHandle(i)));

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/// <summary>
/// Deletes the SDL window.
/// </summary>
void AudioImGuiWindow::DeleteWindow()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

/// <summary>
/// Opens a wav file and adds it to the resources.
/// </summary>
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
        uaudio::Hash hash = m_SoundSystem.LoadSound(path, path);
        delete[] path;
    }
}
