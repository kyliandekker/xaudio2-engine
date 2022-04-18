#include "tools/MasterTool.h"

#include "wave/WaveFile.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

MasterTool::MasterTool(uaudio::AudioSystem &a_AudioSystem, uaudio::SoundSystem &a_SoundSystem) : BaseTool(0, "Actions", "Master Actions"), m_AudioSystem(a_AudioSystem), m_SoundSystem(a_SoundSystem)
{
    uaudio::BUFFERSIZE buffer_size = m_AudioSystem.GetBufferSize();
    for (int i = 0; i < m_BufferSizeOptions.size(); i++)
        if (m_BufferSizeOptions[i] == buffer_size)
            m_BufferSizeSelection = i;
}

void MasterTool::Render()
{
    std::string master_actions = "Master Actions##Master_Actions_01";
    if (ImGui::CollapsingHeader(master_actions.c_str()))
    {
        if (m_AudioSystem.HasPlayback())
        {
            if (ImGui::Button(PAUSE, ImVec2(50, 50)))
                m_AudioSystem.SetPlaybackStatus(false);
        }
        else
        {
            if (ImGui::Button(PLAY, ImVec2(50, 50)))
                m_AudioSystem.SetPlaybackStatus(true);
        }
        ImGui::SameLine();
        if (ImGui::Button(STOP, ImVec2(50, 50)))
        {
            m_AudioSystem.SetPlaybackStatus(false);
            for (int32_t i = 0; i < static_cast<int32_t>(m_AudioSystem.ChannelSize()); i++)
            {
                m_AudioSystem.GetChannel(i)->SetPos(0);
            }
        }

        float panning = m_AudioSystem.GetMasterPanning();
        std::string master_panning = std::string(PANNING) + " Master Panning (affects all channels)";
        if (ImGui::Knob("Panning##Master_Panning", &panning, -1.0f, 1.0f, ImVec2(50, 50), master_panning.c_str(), 0.0f))
        {
            m_AudioSystem.SetMasterPanning(panning);
        }

        ImGui::SameLine();
        float volume = m_AudioSystem.GetMasterVolume();
        std::string master_volume = std::string(VOLUME_UP) + " Master Volume (affects all channels)";
        if (ImGui::Knob("Volume##Master_Volume", &volume, 0, 1, ImVec2(50, 50), master_volume.c_str(), 1.0f))
        {
            m_AudioSystem.SetMasterVolume(volume);
        }

        std::string buffer_size_text = "Buffer Size";
        ImGui::Text("%s", buffer_size_text.c_str());
        if (ImGui::BeginCombo("##Buffer_Size", std::string(m_BufferSizeSelection == -1 ? "CHOOSE BUFFERSIZE" : m_BufferSizeTextOptions[m_BufferSizeSelection]).c_str(), ImGuiComboFlags_PopupAlignLeft))
        {
            for (int n = 0; n < m_BufferSizeTextOptions.size(); n++)
            {
                bool is_selected = n == m_BufferSizeSelection;
                if (ImGui::Selectable(m_BufferSizeTextOptions[n], is_selected))
                {
                    m_BufferSizeSelection = n;
                    m_AudioSystem.SetBufferSize(m_BufferSizeOptions[n]);
                }
            }
            ImGui::EndCombo();
        }
        
        std::string add_sound = std::string(ADD) + " Add Sound";
        if (ImGui::Button(add_sound.c_str()))
            OpenFile();
    }
}

/// <summary>
/// Opens a wav file and adds it to the resources.
/// </summary>
void MasterTool::OpenFile()
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