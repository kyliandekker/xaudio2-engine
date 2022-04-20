#include "tools/MasterTool.h"

#include "wave/WaveFile.h"

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_helpers.h>

MasterTool::MasterTool(uaudio::AudioSystem& a_AudioSystem, uaudio::SoundSystem& a_SoundSystem) : BaseTool(0, "Actions", "Master Actions"), m_AudioSystem(a_AudioSystem), m_SoundSystem(a_SoundSystem)
{
    uaudio::BUFFERSIZE buffer_size = m_AudioSystem.GetBufferSize();
    for (int i = 0; i < m_BufferSizeOptions.size(); i++)
        if (m_BufferSizeOptions[i] == buffer_size)
            m_BufferSizeSelection = i;

    m_ChunkIds.push_back({ uaudio::ACID_CHUNK_ID, false, false });
    m_ChunkIds.push_back({ uaudio::BEXT_CHUNK_ID, false, false });
    m_ChunkIds.push_back({ uaudio::FACT_CHUNK_ID, false, false });
}

void MasterTool::Render()
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
            m_AudioSystem.GetChannel(i)->SetPos(0);
    }

    float panning = m_AudioSystem.GetMasterPanning();
    std::string master_panning = std::string(PANNING) + " Master Panning (affects all channels)";
    if (ImGui::Knob("Panning##Master_Panning", &panning, -1.0f, 1.0f, ImVec2(50, 50), master_panning.c_str(), 0.0f))
        m_AudioSystem.SetMasterPanning(panning);

    ImGui::SameLine();
    float volume = m_AudioSystem.GetMasterVolume();
    std::string master_volume = std::string(VOLUME_UP) + " Master Volume (affects all channels)";
    if (ImGui::Knob("Volume##Master_Volume", &volume, 0, 1, ImVec2(50, 50), master_volume.c_str(), 1.0f))
        m_AudioSystem.SetMasterVolume(volume);

    std::string buffer_size_text = "Buffer Size";
    ImGui::Text("%s", buffer_size_text.c_str());
    if (ImGui::BeginCombo("##Buffer_Size", std::string(m_BufferSizeSelection == -1 ? "CHOOSE BUFFERSIZE" : m_BufferSizeTextOptions[m_BufferSizeSelection]).c_str(), ImGuiComboFlags_PopupAlignLeft))
    {
        for (uint32_t n = 0; n < static_cast<uint32_t>(m_BufferSizeTextOptions.size()); n++)
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

    std::string add_sound = std::string(ADD) + " Load Sound";
    if (ImGui::Button(add_sound.c_str()))
        OpenFile();

    if (ImGui::CollapsingHeader("Extra Options"))
    {
        ImGui::Indent(IMGUI_INDENT);
        for (uint32_t i = 0; i < m_ChunkIds.size(); i++)
        {
            ImGui::CheckboxButton(m_ChunkIds[i].chunk_id.c_str(), &m_ChunkIds[i].selected);
            if (m_ChunkIds[i].removable)
            {
                ImGui::SameLine();
                std::string test = std::string(MINUS) + "##RemoveChunk" + std::to_string(i);
                if (ImGui::Button(test.c_str()))
                    m_ChunkIds.erase(m_ChunkIds.begin() + i);
            }
        }
        static chunk_select select = { "", false, true };
        if (ImGui::InputText("##Add_Chunk", &select.chunk_id))
            if (select.chunk_id.size() > uaudio::CHUNK_ID_SIZE)
                select.chunk_id = std::string(select.chunk_id.substr(0, uaudio::CHUNK_ID_SIZE));
        if (ImGui::Button("Add"))
        {
            bool canAdd = true;
            if (select.chunk_id.size() > uaudio::CHUNK_ID_SIZE)
                select.chunk_id = std::string(select.chunk_id.substr(0, uaudio::CHUNK_ID_SIZE));
            for (auto& m_ChunkId : m_ChunkIds)
	            if (m_ChunkId.chunk_id == select.chunk_id)
                    canAdd = false;
            if (canAdd)
                m_ChunkIds.push_back(select);
        }
        ImGui::Unindent(IMGUI_INDENT);
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

        std::vector<const char*> chunks;
        for (auto& chunk : m_ChunkIds)
            if (chunk.selected)
                chunks.push_back(chunk.chunk_id.c_str());

        UAUDIO_DEFAULT_HASH hash = m_SoundSystem.LoadSound(path, path, chunks);
        delete[] path;
    }
}