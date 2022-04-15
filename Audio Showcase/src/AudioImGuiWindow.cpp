#include "AudioImGuiWindow.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <sdl/SDL_cpuinfo.h>

#include "AudioSDLWindow.h"

#include <imgui/imgui.h>

#include <xaudio2/XAudio2Callback.h>

AudioImGuiWindow::AudioImGuiWindow() = default;

AudioImGuiWindow::AudioImGuiWindow(AudioSDLWindow* a_Window, uaudio::AudioSystem* a_AudioSystem, uaudio::SoundSystem* a_SoundSystem) : m_Window(a_Window), m_AudioSystem(a_AudioSystem), m_SoundSystem(a_SoundSystem)
{
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
/// <param name="a_Context">The SDL context that will be passed to ImGui.</param>
/// <param name="a_Glslversion">The version of GLSL.</param>
/// <returns>Returns 0 if successful.</returns>
uint32_t AudioImGuiWindow::CreateImGui() const
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

#define ICON_MIN_FA 0xf000
#define ICON_MAX_FA 0xf2e0
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
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

            std::string play_button = std::string(PLAY) + " Play##Play_Sound_" + a_WaveFile.GetSoundTitle();
            if (ImGui::Button(play_button.c_str()))
            {
                m_AudioSystem->Play(a_WaveFile);
            }
            std::string remove_sound = std::string(MINUS) + " Remove##Remove_Sound_" + a_WaveFile.GetSoundTitle();
            if (ImGui::Button(remove_sound.c_str()))
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

                std::string panning_text = std::string(PANNING) + " Panning";
                ImGui::Text(panning_text.c_str());
                ImGui::SameLine();
                float panning = a_Channel->GetPanning();
                ImGui::SliderFloat(std::string("###Panning_Channel_" + std::to_string(a_Index)).c_str(), &panning, -1, 1);
                a_Channel->SetPanning(panning);

                std::string volume_text = std::string(VOLUME_UP) + " Volume";
                ImGui::Text(volume_text.c_str());
                ImGui::SameLine();
                float volume = a_Channel->GetVolume();
                ImGui::SliderFloat(std::string("###Volume_Channel_" + std::to_string(a_Index)).c_str(), &volume, 0, 1);
                a_Channel->SetVolume(volume);

                int32_t pos = static_cast<int32_t>(a_Channel->GetPos(uaudio::TIMEUNIT::TIMEUNIT_POS));
                ImGui::Text(std::string(
                    uaudio::WaveFile::FormatDuration(static_cast<float>(a_Channel->GetPos(uaudio::TIMEUNIT::TIMEUNIT_POS)) / static_cast<float>(a_Channel->GetSound().GetWavFormat().fmtChunk.byteRate)) +
                    "/" +
                    uaudio::WaveFile::FormatDuration(uaudio::WaveFile::GetDuration(a_Channel->GetSound().GetWavFormat().dataChunk.chunkSize, a_Channel->GetSound().GetWavFormat().fmtChunk.byteRate)))
                    .c_str());
                if (ImGui::SliderInt(std::string("###Player_" + std::to_string(a_Index)).c_str(), &pos, 0.0f, a_Channel->GetSound().GetWavFormat().dataChunk.chunkSize, ""))
                {
                    uint32_t newtest = pos % m_AudioSystem->GetBufferSize();
                    uint32_t finaltest = pos - newtest;
                    a_Channel->SetPos(finaltest);
                    if (!a_Channel->IsPlaying())
						a_Channel->PlayRanged(finaltest, m_AudioSystem->GetBufferSize());
                }
                if (a_Channel->IsPlaying())
                {
                    std::string pause_button = std::string(PAUSE) + " Pause##Pause_Sound_" + std::to_string(a_Index);
                    if (ImGui::Button(pause_button.c_str()))
                        a_Channel->Pause();
                }
                else
                {
                    std::string play_button = std::string(PLAY) + " Play##Play_Sound_" + std::to_string(a_Index);
                    if (ImGui::Button(play_button.c_str()))
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
    ImGui::NewFrame();
    
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(m_Window->GetWindowSize().x, m_Window->GetWindowSize().y));
    ImGui::Begin("DockSpace", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking);
    ImGui::DockSpace(ImGui::GetID("DockSpace"));
	ImGui::PushFont(m_DefaultFont);

    {
        ImGui::SetNextWindowSize(
            m_ControlsWindow.m_Size,
            ImGuiCond_Always);

        ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoResize);

        ImGui::Dummy(ImVec2(0.0f, 1.0f));
        std::string master_actions = "Master Actions##Master_Actions_01";
        if (ImGui::CollapsingHeader(master_actions.c_str()))
        {
            ImGui::Indent(16.0f);

            float panning = m_AudioSystem->GetMasterPanning();
            std::string master_panning = std::string(PANNING) + " Master Panning";
            ImGui::Text(master_panning.c_str());
            ImGui::SameLine();
            ImGui::SliderFloat("###Panning_Master_0", &panning, -1, 1);
            m_AudioSystem->SetMasterPanning(panning);

            float volume = m_AudioSystem->GetMasterVolume();
            std::string master_volume = std::string(VOLUME_UP) + " Master Volume";
            ImGui::Text(master_volume.c_str());
            ImGui::SameLine();
            ImGui::SliderFloat("###Volume_Master_0", &volume, 0, 1);
            m_AudioSystem->SetMasterVolume(volume);

            if (m_AudioSystem->HasPlayback())
            {
                std::string pause_button = std::string(PAUSE) + " Pause Whole Playback##Pause_Whole_Playback";
                if (ImGui::Button(pause_button.c_str()))
                    m_AudioSystem->SetPlaybackStatus(false);
            }
            else
            {
                std::string play_button = std::string(PLAY) + " Resume Whole Playback##Resume_Whole_Playback";
                if (ImGui::Button(play_button.c_str()))
                    m_AudioSystem->SetPlaybackStatus(true);
            }
            std::string add_sound = std::string(ADD) + " Add Sound";
            if (ImGui::Button(add_sound.c_str()))
                OpenFile();
        }

        ImGui::End();
    }

    {
        ImGui::Begin("Sounds", nullptr, ImGuiWindowFlags_NoResize);

        ImGui::Dummy(ImVec2(0.0f, 1.0f));

        for (auto* sound : m_SoundSystem->GetSounds())
            RenderSound(*sound);

        ImGui::End();
    }

	{
        ImGui::Begin("Channels", nullptr, ImGuiWindowFlags_NoResize);

        ImGui::Dummy(ImVec2(0.0f, 1.0f));

        for (uint32_t i = 0; i < m_AudioSystem->ChannelSize(); i++)
            RenderChannel(i, m_AudioSystem->GetChannel(uaudio::ChannelHandle(i)));

        ImGui::End();
    }

    ImGui::PopFont();
    ImGui::End();

    ImGui::Render();
}

/// <summary>
/// Deletes the SDL window.
/// </summary>
void AudioImGuiWindow::DeleteWindow()
{
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
        uaudio::Hash hash = m_SoundSystem->LoadSound(path, path);
        delete[] path;
    }
}
