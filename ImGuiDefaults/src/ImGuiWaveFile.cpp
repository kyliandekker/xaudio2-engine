#include "ImGuiWaveFile.h"

#include <string>
#include <imgui/imgui.h>
#include <xaudio2_engine/wav/WaveFile.h>

ImGuiWaveFile::ImGuiWaveFile(WaveFile& a_WaveFile)
{
	m_WaveFile = &a_WaveFile;
}

void ImGuiWaveFile::RenderElement(uint32_t a_Index)
{
    if (m_WaveFile == nullptr)
        return;

    if (ImGui::CollapsingHeader(m_WaveFile->GetSoundTitle()))
    {
        ImGui::Indent(16.0f);

        if (ImGui::CollapsingHeader(std::string("File Info###FileInfo_" + std::to_string(a_Index)).c_str()))
        {
            ImGui::Indent(16.0f);

            if (ImGui::CollapsingHeader(std::string("Deeper File Info###DeeperFileInfo_" + std::to_string(a_Index)).c_str()))
            {
                ImGui::Indent(16.0f);
                ShowValue("Block Align: ", std::string(std::to_string(m_WaveFile->GetWavFormat().fmtChunk.blockAlign)).c_str());
                ShowValue("Format: ", std::string(std::to_string(m_WaveFile->GetWavFormat().dataChunk.chunkSize)).c_str());
                ImGui::Unindent(16.0f);
            }
            ShowValue("Name: ", m_WaveFile->GetSoundTitle());
            ShowValue("Format: ", std::to_string(m_WaveFile->GetWavFormat().fmtChunk.audioFormat).c_str());
            ShowValue("Number Of Channels: ", std::string(std::to_string(m_WaveFile->GetWavFormat().fmtChunk.numChannels) + (m_WaveFile->GetWavFormat().fmtChunk.numChannels == 1 ? " (mono)" : " (stereo)")).c_str());
            ShowValue("Sample Rate: ", std::to_string(m_WaveFile->GetWavFormat().fmtChunk.sampleRate).c_str());
            ShowValue("Byte Rate: ", std::string(std::to_string(m_WaveFile->GetWavFormat().fmtChunk.byteRate) + " Hz").c_str());
            ShowValue("Bits Per Sample: ", std::to_string(m_WaveFile->GetWavFormat().fmtChunk.bitsPerSample).c_str());
            ShowValue("Duration (sec): ", std::string(std::to_string(m_WaveFile->GetDuration(m_WaveFile->GetWavFormat().dataChunk.chunkSize, m_WaveFile->GetWavFormat().fmtChunk.byteRate)) + " secs").c_str());
            if (strcmp(std::string(&m_WaveFile->GetWavFormat().bextChunk.origination_date[0], &m_WaveFile->GetWavFormat().bextChunk.origination_date[0] + std::size(m_WaveFile->GetWavFormat().bextChunk.origination_date)).c_str(), "") != 0)
                ShowValue("Origination Date: ", std::string(&m_WaveFile->GetWavFormat().bextChunk.origination_date[0], &m_WaveFile->GetWavFormat().bextChunk.origination_date[0] + std::size(m_WaveFile->GetWavFormat().bextChunk.origination_date)).c_str());
            if (strcmp(std::string(&m_WaveFile->GetWavFormat().bextChunk.origination_time[0], &m_WaveFile->GetWavFormat().bextChunk.origination_time[0] + std::size(m_WaveFile->GetWavFormat().bextChunk.origination_time)).c_str(), "") != 0)
                ShowValue("Origination Time: ", std::string(&m_WaveFile->GetWavFormat().bextChunk.origination_time[0], &m_WaveFile->GetWavFormat().bextChunk.origination_time[0] + std::size(m_WaveFile->GetWavFormat().bextChunk.origination_time)).c_str());
            ShowValue("Duration: ", WaveFile::FormatDuration(m_WaveFile->GetDuration(m_WaveFile->GetWavFormat().dataChunk.chunkSize, m_WaveFile->GetWavFormat().fmtChunk.byteRate)).c_str());
            if (m_WaveFile->GetWavFormat().filledAcidChunk)
                ShowValue("Tempo: ", std::to_string(m_WaveFile->GetWavFormat().acidChunk.tempo).c_str());
            else
            {
                const ImVec4 color = ImVec4(GetRGBColor(255), GetRGBColor(0), GetRGBColor(0), 1.0f);
                ImGui::TextColored(color, "%s\n", "WARNING: NO TEMPO INFO FOUND!");

                ImGui::InputFloat(std::string("Tempo###Tempo" + std::to_string(a_Index)).c_str(), &m_Tempo, 10.0f, 9000.0f);
                if (ImGui::Button("Add tempo info"))
                    m_WaveFile->AddAcidChunk(m_Tempo);
            }
            if (ImGui::Button(std::string("Remove Sound###RemoveSound_" + std::to_string(a_Index)).c_str()))
            {
                //m_Sounds.erase(m_Sounds.begin() + i);
                //m_Channels.erase(m_Channels.begin() + i);
            }

            ImGui::Unindent(16.0f);
        }
        ImGui::Unindent(16.0f);
    }
}