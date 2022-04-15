#include "tools/MasterTool.h"

#include "wav/WaveFile.h"

#include <imgui/imgui.h>

MasterTool::MasterTool(uaudio::AudioSystem& a_AudioSystem, uaudio::SoundSystem& a_SoundSystem) : BaseTool(0, "Actions", "Master Actions"), m_AudioSystem(a_AudioSystem), m_SoundSystem(a_SoundSystem)
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
        ImGui::Indent(16.0f);

        float panning = m_AudioSystem.GetMasterPanning();
        std::string master_panning = std::string(PANNING) + " Master Panning";
        ImGui::Text(master_panning.c_str());
        ImGui::SameLine();
        ImGui::SliderFloat("###Panning_Master_0", &panning, -1, 1);
        m_AudioSystem.SetMasterPanning(panning);

        float volume = m_AudioSystem.GetMasterVolume();
        std::string master_volume = std::string(VOLUME_UP) + " Master Volume";
        ImGui::Text(master_volume.c_str());
        ImGui::SameLine();
        ImGui::SliderFloat("###Volume_Master_0", &volume, 0, 1);
        m_AudioSystem.SetMasterVolume(volume);

        std::string buffer_size_text = "Buffer Size";
        ImGui::Text(buffer_size_text.c_str());
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

        if (m_AudioSystem.HasPlayback())
        {
            std::string pause_button = std::string(PAUSE) + " Pause Whole Playback##Pause_Whole_Playback";
            if (ImGui::Button(pause_button.c_str()))
                m_AudioSystem.SetPlaybackStatus(false);
        }
        else
        {
            std::string play_button = std::string(PLAY) + " Resume Whole Playback##Resume_Whole_Playback";
            if (ImGui::Button(play_button.c_str()))
                m_AudioSystem.SetPlaybackStatus(true);
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
    TCHAR sz_file[260] = { 0 };

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
        char* path = new char[wcslen(ofn.lpstrFile) + 1];
        wsprintfA(path, "%S", ofn.lpstrFile);
        uaudio::Hash hash = m_SoundSystem.LoadSound(path, path);
        delete[] path;
    }
}